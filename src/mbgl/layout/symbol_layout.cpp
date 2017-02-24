#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/layout/merge_lines.hpp>
#include <mbgl/layout/clip_lines.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/style/bucket_parameters.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/text/get_anchors.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/utf.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/i18n.hpp>
#include <mbgl/math/clamp.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/logging.hpp>

#include <mapbox/polylabel.hpp>

namespace mbgl {

using namespace style;

SymbolLayout::SymbolLayout(const BucketParameters& parameters,
                           const std::vector<const Layer*>& layers,
                           const GeometryTileLayer& sourceLayer,
                           SpriteAtlas& spriteAtlas_)
    : sourceLayerName(sourceLayer.getName()),
      bucketName(layers.at(0)->getID()),
      overscaling(parameters.tileID.overscaleFactor()),
      zoom(parameters.tileID.overscaledZ),
      mode(parameters.mode),
      spriteAtlas(spriteAtlas_),
      tileSize(util::tileSize * overscaling),
      tilePixelRatio(float(util::EXTENT) / tileSize) {

    const SymbolLayer::Impl& leader = *layers.at(0)->as<SymbolLayer>()->impl;

    layout = leader.layout.evaluate(PropertyEvaluationParameters(zoom));

    if (layout.get<IconRotationAlignment>() == AlignmentType::Auto) {
        if (layout.get<SymbolPlacement>() == SymbolPlacementType::Line) {
            layout.get<IconRotationAlignment>() = AlignmentType::Map;
        } else {
            layout.get<IconRotationAlignment>() = AlignmentType::Viewport;
        }
    }

    if (layout.get<TextRotationAlignment>() == AlignmentType::Auto) {
        if (layout.get<SymbolPlacement>() == SymbolPlacementType::Line) {
            layout.get<TextRotationAlignment>() = AlignmentType::Map;
        } else {
            layout.get<TextRotationAlignment>() = AlignmentType::Viewport;
        }
    }

    // If unspecified `text-pitch-alignment` inherits `text-rotation-alignment`
    if (layout.get<TextPitchAlignment>() == AlignmentType::Auto) {
        layout.get<TextPitchAlignment>() = layout.get<TextRotationAlignment>();
    }

    textMaxSize = leader.layout.evaluate<TextSize>(PropertyEvaluationParameters(18));

    layout.get<IconSize>() = leader.layout.evaluate<IconSize>(PropertyEvaluationParameters(zoom + 1));
    layout.get<TextSize>() = leader.layout.evaluate<TextSize>(PropertyEvaluationParameters(zoom + 1));
          
    const bool hasTextField = layout.get<TextField>().match(
        [&] (const std::string& s) { return !s.empty(); },
        [&] (const auto&) { return true; }
    );

    const bool hasText = !layout.get<TextFont>().empty() && hasTextField;

    const bool hasIcon = !layout.get<IconImage>().empty();

    if (!hasText && !hasIcon) {
        return;
    }

    for (const auto& layer : layers) {
        layerPaintProperties.emplace(layer->getID(), layer->as<SymbolLayer>()->impl->paint.evaluated);
    }

    // Determine and load glyph ranges
    const size_t featureCount = sourceLayer.featureCount();
    for (size_t i = 0; i < featureCount; ++i) {
        auto feature = sourceLayer.getFeature(i);
        if (!leader.filter(feature->getType(), feature->getID(), [&] (const auto& key) { return feature->getValue(key); }))
            continue;

        SymbolFeature ft;
        ft.index = i;

        auto getValue = [&feature](const std::string& key) -> std::string {
            auto value = feature->getValue(key);
            if (!value)
                return std::string();
            if (value->is<std::string>())
                return value->get<std::string>();
            if (value->is<bool>())
                return value->get<bool>() ? "true" : "false";
            if (value->is<int64_t>())
                return util::toString(value->get<int64_t>());
            if (value->is<uint64_t>())
                return util::toString(value->get<uint64_t>());
            if (value->is<double>())
                return util::toString(value->get<double>());
            return "null";
        };
        
        if (hasText) {
            std::string u8string = layout.evaluate<TextField>(zoom, *feature);
            if (layout.get<TextField>().isConstant()) {
                u8string = util::replaceTokens(u8string, getValue);
            }
            
            auto textTransform = layout.evaluate<TextTransform>(zoom, *feature);

            if (textTransform == TextTransformType::Uppercase) {
                u8string = platform::uppercase(u8string);
            } else if (textTransform == TextTransformType::Lowercase) {
                u8string = platform::lowercase(u8string);
            }

            ft.text = applyArabicShaping(util::utf8_to_utf16::convert(u8string));

            // Loop through all characters of this text and collect unique codepoints.
            for (char16_t chr : *ft.text) {
                ranges.insert(getGlyphRange(chr));
                if (char16_t verticalChr = util::i18n::verticalizePunctuation(chr)) {
                    ranges.insert(getGlyphRange(verticalChr));
                }
            }
        }

        if (hasIcon) {
            ft.icon = util::replaceTokens(layout.get<IconImage>(), getValue);
            ft.iconOffset = layout.evaluate<IconOffset>(zoom, *feature);
            ft.iconRotation = layout.evaluate<IconRotate>(zoom, *feature) * util::DEG2RAD;
        }

        if (ft.text || ft.icon) {
            ft.type = feature->getType();
            ft.geometry = feature->getGeometries();
            features.push_back(std::move(ft));
        }
    }

    if (layout.get<SymbolPlacement>() == SymbolPlacementType::Line) {
        util::mergeLines(features);
    }
}

bool SymbolLayout::hasSymbolInstances() const {
    return !symbolInstances.empty();
}

bool SymbolLayout::canPrepare(GlyphAtlas& glyphAtlas) {
    const bool hasTextField = layout.get<TextField>().match(
        [&] (const std::string& s) { return !s.empty(); },
        [&] (const auto&) { return true; }
    );

    if (hasTextField && !layout.get<TextFont>().empty() && !glyphAtlas.hasGlyphRanges(layout.get<TextFont>(), ranges)) {
        return false;
    }

    if (!layout.get<IconImage>().empty() && !spriteAtlas.isLoaded()) {
        return false;
    }

    return true;
}

void SymbolLayout::prepare(uintptr_t tileUID,
                           GlyphAtlas& glyphAtlas) {
    float horizontalAlign = 0.5;
    float verticalAlign = 0.5;

    switch (layout.get<TextAnchor>()) {
        case TextAnchorType::Top:
        case TextAnchorType::Bottom:
        case TextAnchorType::Center:
            break;
        case TextAnchorType::Right:
        case TextAnchorType::TopRight:
        case TextAnchorType::BottomRight:
            horizontalAlign = 1;
            break;
        case TextAnchorType::Left:
        case TextAnchorType::TopLeft:
        case TextAnchorType::BottomLeft:
            horizontalAlign = 0;
            break;
    }

    switch (layout.get<TextAnchor>()) {
        case TextAnchorType::Left:
        case TextAnchorType::Right:
        case TextAnchorType::Center:
            break;
        case TextAnchorType::Bottom:
        case TextAnchorType::BottomLeft:
        case TextAnchorType::BottomRight:
            verticalAlign = 1;
            break;
        case TextAnchorType::Top:
        case TextAnchorType::TopLeft:
        case TextAnchorType::TopRight:
            verticalAlign = 0;
            break;
    }

    const float justify = layout.get<TextJustify>() == TextJustifyType::Right ? 1 :
        layout.get<TextJustify>() == TextJustifyType::Left ? 0 :
        0.5;

    auto glyphSet = glyphAtlas.getGlyphSet(layout.get<TextFont>());

    const bool textAlongLine = layout.get<TextRotationAlignment>() == AlignmentType::Map &&
        layout.get<SymbolPlacement>() == SymbolPlacementType::Line;

    for (const auto& feature : features) {
        if (feature.geometry.empty()) continue;

        std::pair<Shaping, Shaping> shapedTextOrientations;
        PositionedIcon shapedIcon;
        GlyphPositions face;

        // if feature has text, shape the text
        if (feature.text) {
            auto getShaping = [&] (const std::u16string& text, WritingModeType writingMode) {
                const float oneEm = 24.0f;
                const Shaping result = glyphSet->getShaping(
                    /* string */ text,
                    /* maxWidth: ems */ layout.get<SymbolPlacement>() != SymbolPlacementType::Line ?
                        layout.get<TextMaxWidth>() * oneEm : 0,
                    /* lineHeight: ems */ layout.get<TextLineHeight>() * oneEm,
                    /* horizontalAlign */ horizontalAlign,
                    /* verticalAlign */ verticalAlign,
                    /* justify */ justify,
                    /* spacing: ems */ layout.get<TextLetterSpacing>() * oneEm,
                    /* translate */ Point<float>(layout.get<TextOffset>()[0], layout.get<TextOffset>()[1]),
                    /* verticalHeight */ oneEm,
                    /* writingMode */ writingMode,
                    /* bidirectional algorithm object */ bidi);

                // Add the glyphs we need for this label to the glyph atlas.
                if (result) {
                    glyphAtlas.addGlyphs(tileUID, text, layout.get<TextFont>(), glyphSet, face);
                }

                return result;
            };

            shapedTextOrientations.first = getShaping(*feature.text, WritingModeType::Horizontal);

            if (util::i18n::allowsVerticalWritingMode(*feature.text) && textAlongLine) {
                shapedTextOrientations.second = getShaping(util::i18n::verticalizePunctuation(*feature.text), WritingModeType::Vertical);
            }
        }

        // if feature has icon, get sprite atlas position
        if (feature.icon) {
            auto image = spriteAtlas.getIcon(*feature.icon);
            if (image) {
                shapedIcon = shapeIcon(*image, feature);
                assert((*image).spriteImage);
                if ((*image).spriteImage->sdf) {
                    sdfIcons = true;
                }
                if ((*image).relativePixelRatio != 1.0f) {
                    iconsNeedLinear = true;
                } else if (layout.get<IconRotate>().constantOr(1) != 0) {
                    iconsNeedLinear = true;
                }
            }
        }

        // if either shapedText or icon position is present, add the feature
        if (shapedTextOrientations.first || shapedIcon) {
            addFeature(feature, shapedTextOrientations, shapedIcon, face);
        }
    }

    features.clear();
}

void SymbolLayout::addFeature(const SymbolFeature& feature,
                              const std::pair<Shaping, Shaping>& shapedTextOrientations,
                              const PositionedIcon& shapedIcon,
                              const GlyphPositions& face) {
    const float minScale = 0.5f;
    const float glyphSize = 24.0f;

    const float fontScale = layout.get<TextSize>() / glyphSize;
    const float textBoxScale = tilePixelRatio * fontScale;
    const float textMaxBoxScale = tilePixelRatio * textMaxSize / glyphSize;
    const float iconBoxScale = tilePixelRatio * layout.get<IconSize>();
    const float symbolSpacing = tilePixelRatio * layout.get<SymbolSpacing>();
    const bool avoidEdges = layout.get<SymbolAvoidEdges>() && layout.get<SymbolPlacement>() != SymbolPlacementType::Line;
    const float textPadding = layout.get<TextPadding>() * tilePixelRatio;
    const float iconPadding = layout.get<IconPadding>() * tilePixelRatio;
    const float textMaxAngle = layout.get<TextMaxAngle>() * util::DEG2RAD;
    const SymbolPlacementType textPlacement = layout.get<TextRotationAlignment>() != AlignmentType::Map
                                                  ? SymbolPlacementType::Point
                                                  : layout.get<SymbolPlacement>();
    const SymbolPlacementType iconPlacement = layout.get<IconRotationAlignment>() != AlignmentType::Map
                                                  ? SymbolPlacementType::Point
                                                  : layout.get<SymbolPlacement>();
    const float textRepeatDistance = symbolSpacing / 2;
    IndexedSubfeature indexedFeature = {feature.index, sourceLayerName, bucketName, symbolInstances.size()};

    auto addSymbolInstance = [&] (const GeometryCoordinates& line, Anchor& anchor) {
        // https://github.com/mapbox/vector-tile-spec/tree/master/2.1#41-layers
        // +-------------------+ Symbols with anchors located on tile edges
        // |(0,0)             || are duplicated on neighbor tiles.
        // |                  ||
        // |                  || In continuous mode, to avoid overdraw we
        // |                  || skip symbols located on the extent edges.
        // |       Tile       || In still mode, we include the features in
        // |                  || the buffers for both tiles and clip them
        // |                  || at draw time.
        // |                  ||
        // +-------------------| In this scenario, the inner bounding box
        // +-------------------+ is called 'withinPlus0', and the outer
        //       (extent,extent) is called 'inside'.
        const bool withinPlus0 = anchor.point.x >= 0 && anchor.point.x < util::EXTENT && anchor.point.y >= 0 && anchor.point.y < util::EXTENT;
        const bool inside = withinPlus0 || anchor.point.x == util::EXTENT || anchor.point.y == util::EXTENT;

        if (avoidEdges && !inside) return;

        const bool addToBuffers = mode == MapMode::Still || withinPlus0;

        symbolInstances.emplace_back(anchor, line, shapedTextOrientations, shapedIcon, layout, addToBuffers, symbolInstances.size(),
                textBoxScale, textPadding, textPlacement,
                iconBoxScale, iconPadding, iconPlacement,
                face, indexedFeature);
    };

    if (layout.get<SymbolPlacement>() == SymbolPlacementType::Line) {
        auto clippedLines = util::clipLines(feature.geometry, 0, 0, util::EXTENT, util::EXTENT);
        for (const auto& line : clippedLines) {
            Anchors anchors = getAnchors(line,
                                         symbolSpacing,
                                         textMaxAngle,
                                         (shapedTextOrientations.second ?: shapedTextOrientations.first).left,
                                         (shapedTextOrientations.second ?: shapedTextOrientations.first).right,
                                         shapedIcon.left,
                                         shapedIcon.right,
                                         glyphSize,
                                         textMaxBoxScale,
                                         overscaling);

            for (auto& anchor : anchors) {
                if (!shapedTextOrientations.first || !anchorIsTooClose(shapedTextOrientations.first.text, textRepeatDistance, anchor)) {
                    addSymbolInstance(line, anchor);
                }
            }
        }
    } else if (feature.type == FeatureType::Polygon) {
        for (const auto& polygon : classifyRings(feature.geometry)) {
            Polygon<double> poly;
            for (const auto& ring : polygon) {
                LinearRing<double> r;
                for (const auto& p : ring) {
                    r.push_back(convertPoint<double>(p));
                }
                poly.push_back(r);
            }

            // 1 pixel worth of precision, in tile coordinates
            auto poi = mapbox::polylabel(poly, double(util::EXTENT / util::tileSize));
            Anchor anchor(poi.x, poi.y, 0, minScale);
            addSymbolInstance(polygon[0], anchor);
        }
    } else if (feature.type == FeatureType::LineString) {
        for (const auto& line : feature.geometry) {
            Anchor anchor(line[0].x, line[0].y, 0, minScale);
            addSymbolInstance(line, anchor);
        }
    } else if (feature.type == FeatureType::Point) {
        for (const auto& points : feature.geometry) {
            for (const auto& point : points) {
                Anchor anchor(point.x, point.y, 0, minScale);
                addSymbolInstance({point}, anchor);
            }
        }
    }
}

bool SymbolLayout::anchorIsTooClose(const std::u16string& text, const float repeatDistance, const Anchor& anchor) {
    if (compareText.find(text) == compareText.end()) {
        compareText.emplace(text, Anchors());
    } else {
        auto otherAnchors = compareText.find(text)->second;
        for (const Anchor& otherAnchor : otherAnchors) {
            if (util::dist<float>(anchor.point, otherAnchor.point) < repeatDistance) {
                return true;
            }
        }
    }
    compareText[text].push_back(anchor);
    return false;
}

std::unique_ptr<SymbolBucket> SymbolLayout::place(CollisionTile& collisionTile) {
    auto bucket = std::make_unique<SymbolBucket>(layout, layerPaintProperties, zoom, sdfIcons, iconsNeedLinear);

    // Calculate which labels can be shown and when they can be shown and
    // create the bufers used for rendering.

    const SymbolPlacementType textPlacement = layout.get<TextRotationAlignment>() != AlignmentType::Map
                                                  ? SymbolPlacementType::Point
                                                  : layout.get<SymbolPlacement>();
    const SymbolPlacementType iconPlacement = layout.get<IconRotationAlignment>() != AlignmentType::Map
                                                  ? SymbolPlacementType::Point
                                                  : layout.get<SymbolPlacement>();

    const bool mayOverlap = layout.get<TextAllowOverlap>() || layout.get<IconAllowOverlap>() ||
        layout.get<TextIgnorePlacement>() || layout.get<IconIgnorePlacement>();

    // Sort symbols by their y position on the canvas so that they lower symbols
    // are drawn on top of higher symbols.
    // Don't sort symbols that won't overlap because it isn't necessary and
    // because it causes more labels to pop in and out when rotating.
    if (mayOverlap) {
        const float sin = std::sin(collisionTile.config.angle);
        const float cos = std::cos(collisionTile.config.angle);

        std::sort(symbolInstances.begin(), symbolInstances.end(), [sin, cos](SymbolInstance &a, SymbolInstance &b) {
            const int32_t aRotated = sin * a.point.x + cos * a.point.y;
            const int32_t bRotated = sin * b.point.x + cos * b.point.y;
            return aRotated != bRotated ?
                aRotated < bRotated :
                a.index > b.index;
        });
    }

    for (SymbolInstance &symbolInstance : symbolInstances) {

        const bool hasText = symbolInstance.hasText;
        const bool hasIcon = symbolInstance.hasIcon;

        const bool iconWithoutText = layout.get<TextOptional>() || !hasText;
        const bool textWithoutIcon = layout.get<IconOptional>() || !hasIcon;

        // Calculate the scales at which the text and icon can be placed without collision.

        float glyphScale = hasText ?
            collisionTile.placeFeature(symbolInstance.textCollisionFeature,
                    layout.get<TextAllowOverlap>(), layout.get<SymbolAvoidEdges>()) :
            collisionTile.minScale;
        float iconScale = hasIcon ?
            collisionTile.placeFeature(symbolInstance.iconCollisionFeature,
                    layout.get<IconAllowOverlap>(), layout.get<SymbolAvoidEdges>()) :
            collisionTile.minScale;


        // Combine the scales for icons and text.

        if (!iconWithoutText && !textWithoutIcon) {
            iconScale = glyphScale = util::max(iconScale, glyphScale);
        } else if (!textWithoutIcon && glyphScale) {
            glyphScale = util::max(iconScale, glyphScale);
        } else if (!iconWithoutText && iconScale) {
            iconScale = util::max(iconScale, glyphScale);
        }


        // Insert final placement into collision tree and add glyphs/icons to buffers

        if (hasText) {
            collisionTile.insertFeature(symbolInstance.textCollisionFeature, glyphScale, layout.get<TextIgnorePlacement>());
            if (glyphScale < collisionTile.maxScale) {
                addSymbols(
                    bucket->text, symbolInstance.glyphQuads, glyphScale,
                    layout.get<TextKeepUpright>(), textPlacement, collisionTile.config.angle, symbolInstance.writingModes);
            }
        }

        if (hasIcon) {
            collisionTile.insertFeature(symbolInstance.iconCollisionFeature, iconScale, layout.get<IconIgnorePlacement>());
            if (iconScale < collisionTile.maxScale) {
                addSymbols(
                    bucket->icon, symbolInstance.iconQuads, iconScale,
                    layout.get<IconKeepUpright>(), iconPlacement, collisionTile.config.angle, symbolInstance.writingModes);
            }
        }
    }

    if (collisionTile.config.debug) {
        addToDebugBuffers(collisionTile, *bucket);
    }

    return bucket;
}

template <typename Buffer>
void SymbolLayout::addSymbols(Buffer &buffer, const SymbolQuads &symbols, float scale, const bool keepUpright, const style::SymbolPlacementType placement, const float placementAngle, WritingModeType writingModes) {
    constexpr const uint16_t vertexLength = 4;
    const float placementZoom = util::max(util::log2(scale) + zoom, 0.0f);

    for (const auto& symbol : symbols) {
        const auto &tl = symbol.tl;
        const auto &tr = symbol.tr;
        const auto &bl = symbol.bl;
        const auto &br = symbol.br;
        const auto &tex = symbol.tex;

        float minZoom = util::max(zoom + util::log2(symbol.minScale), placementZoom);
        float maxZoom = util::min(zoom + util::log2(symbol.maxScale), util::MAX_ZOOM_F);
        const auto &anchorPoint = symbol.anchorPoint;

        // drop incorrectly oriented glyphs
        const float a = std::fmod(symbol.anchorAngle + placementAngle + M_PI, M_PI * 2);
        if (writingModes & WritingModeType::Vertical) {
            if (placement == style::SymbolPlacementType::Line && symbol.writingMode == WritingModeType::Vertical) {
                if (keepUpright && placement == style::SymbolPlacementType::Line && (a <= (M_PI * 5 / 4) || a > (M_PI * 7 / 4)))
                    continue;
            } else if (keepUpright && placement == style::SymbolPlacementType::Line && (a <= (M_PI * 3 / 4) || a > (M_PI * 5 / 4)))
                continue;
        } else if (keepUpright && placement == style::SymbolPlacementType::Line &&
            (a <= M_PI / 2 || a > M_PI * 3 / 2)) {
            continue;
        }

        if (maxZoom <= minZoom)
            continue;

        // Lower min zoom so that while fading out the label
        // it can be shown outside of collision-free zoom levels
        if (minZoom == placementZoom) {
            minZoom = 0;
        }

        if (buffer.segments.empty() || buffer.segments.back().vertexLength + vertexLength > std::numeric_limits<uint16_t>::max()) {
            buffer.segments.emplace_back(buffer.vertices.vertexSize(), buffer.triangles.indexSize());
        }

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        auto& segment = buffer.segments.back();
        assert(segment.vertexLength <= std::numeric_limits<uint16_t>::max());
        uint16_t index = segment.vertexLength;

        // Encode angle of glyph
        uint8_t glyphAngle = std::round((symbol.glyphAngle / (M_PI * 2)) * 256);

        // coordinates (2 triangles)
        buffer.vertices.emplace_back(SymbolLayoutAttributes::vertex(anchorPoint, tl, tex.x, tex.y,
                            minZoom, maxZoom, placementZoom, glyphAngle));
        buffer.vertices.emplace_back(SymbolLayoutAttributes::vertex(anchorPoint, tr, tex.x + tex.w, tex.y,
                            minZoom, maxZoom, placementZoom, glyphAngle));
        buffer.vertices.emplace_back(SymbolLayoutAttributes::vertex(anchorPoint, bl, tex.x, tex.y + tex.h,
                            minZoom, maxZoom, placementZoom, glyphAngle));
        buffer.vertices.emplace_back(SymbolLayoutAttributes::vertex(anchorPoint, br, tex.x + tex.w, tex.y + tex.h,
                            minZoom, maxZoom, placementZoom, glyphAngle));

        // add the two triangles, referencing the four coordinates we just inserted.
        buffer.triangles.emplace_back(index + 0, index + 1, index + 2);
        buffer.triangles.emplace_back(index + 1, index + 2, index + 3);

        segment.vertexLength += vertexLength;
        segment.indexLength += 6;
    }
}

void SymbolLayout::addToDebugBuffers(CollisionTile& collisionTile, SymbolBucket& bucket) {

    if (!hasSymbolInstances()) {
        return;
    }

    const float yStretch = collisionTile.yStretch;

    auto& collisionBox = bucket.collisionBox;

    for (const SymbolInstance &symbolInstance : symbolInstances) {
        auto populateCollisionBox = [&](const auto& feature) {
            for (const CollisionBox &box : feature.boxes) {
                auto& anchor = box.anchor;

                Point<float> tl{box.x1, box.y1 * yStretch};
                Point<float> tr{box.x2, box.y1 * yStretch};
                Point<float> bl{box.x1, box.y2 * yStretch};
                Point<float> br{box.x2, box.y2 * yStretch};
                tl = util::matrixMultiply(collisionTile.reverseRotationMatrix, tl);
                tr = util::matrixMultiply(collisionTile.reverseRotationMatrix, tr);
                bl = util::matrixMultiply(collisionTile.reverseRotationMatrix, bl);
                br = util::matrixMultiply(collisionTile.reverseRotationMatrix, br);

                const float maxZoom = util::clamp(zoom + util::log2(box.maxScale), util::MIN_ZOOM_F, util::MAX_ZOOM_F);
                const float placementZoom = util::clamp(zoom + util::log2(box.placementScale), util::MIN_ZOOM_F, util::MAX_ZOOM_F);

                static constexpr std::size_t vertexLength = 4;
                static constexpr std::size_t indexLength = 8;

                if (collisionBox.segments.empty() || collisionBox.segments.back().vertexLength + vertexLength > std::numeric_limits<uint16_t>::max()) {
                    collisionBox.segments.emplace_back(collisionBox.vertices.vertexSize(), collisionBox.lines.indexSize());
                }

                auto& segment = collisionBox.segments.back();
                uint16_t index = segment.vertexLength;

                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, tl, maxZoom, placementZoom));
                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, tr, maxZoom, placementZoom));
                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, br, maxZoom, placementZoom));
                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, bl, maxZoom, placementZoom));

                collisionBox.lines.emplace_back(index + 0, index + 1);
                collisionBox.lines.emplace_back(index + 1, index + 2);
                collisionBox.lines.emplace_back(index + 2, index + 3);
                collisionBox.lines.emplace_back(index + 3, index + 0);

                segment.vertexLength += vertexLength;
                segment.indexLength += indexLength;
            }
        };
        populateCollisionBox(symbolInstance.textCollisionFeature);
        populateCollisionBox(symbolInstance.iconCollisionFeature);
    }
}

} // namespace mbgl
