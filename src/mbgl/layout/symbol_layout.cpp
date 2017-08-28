#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/layout/merge_lines.hpp>
#include <mbgl/layout/clip_lines.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/text/get_anchors.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/text/shaping.hpp>
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
#include <mbgl/tile/geometry_tile_data.hpp>

#include <mapbox/polylabel.hpp>

namespace mbgl {

using namespace style;

template <class Property>
static bool has(const style::SymbolLayoutProperties::PossiblyEvaluated& layout) {
    return layout.get<Property>().match(
        [&] (const std::string& s) { return !s.empty(); },
        [&] (const auto&) { return true; }
    );
}

SymbolLayout::SymbolLayout(const BucketParameters& parameters,
                           const std::vector<const RenderLayer*>& layers,
                           std::unique_ptr<GeometryTileLayer> sourceLayer_,
                           ImageDependencies& imageDependencies,
                           GlyphDependencies& glyphDependencies)
    : sourceLayer(std::move(sourceLayer_)),
      bucketName(layers.at(0)->getID()),
      overscaling(parameters.tileID.overscaleFactor()),
      zoom(parameters.tileID.overscaledZ),
      mode(parameters.mode),
      pixelRatio(parameters.pixelRatio),
      tileSize(util::tileSize * overscaling),
      tilePixelRatio(float(util::EXTENT) / tileSize),
      textSize(layers.at(0)->as<RenderSymbolLayer>()->impl().layout.get<TextSize>()),
      iconSize(layers.at(0)->as<RenderSymbolLayer>()->impl().layout.get<IconSize>())
    {

    const SymbolLayer::Impl& leader = layers.at(0)->as<RenderSymbolLayer>()->impl();

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

    // If unspecified `*-pitch-alignment` inherits `*-rotation-alignment`
    if (layout.get<TextPitchAlignment>() == AlignmentType::Auto) {
        layout.get<TextPitchAlignment>() = layout.get<TextRotationAlignment>();
    }
    if (layout.get<IconPitchAlignment>() == AlignmentType::Auto) {
        layout.get<IconPitchAlignment>() = layout.get<IconRotationAlignment>();
    }

    const bool hasText = has<TextField>(layout) && !layout.get<TextFont>().empty();
    const bool hasIcon = has<IconImage>(layout);

    if (!hasText && !hasIcon) {
        return;
    }

    for (const auto& layer : layers) {
        layerPaintProperties.emplace(layer->getID(), std::make_pair(
            layer->as<RenderSymbolLayer>()->iconPaintProperties(),
            layer->as<RenderSymbolLayer>()->textPaintProperties()
        ));
    }

    // Determine glyph dependencies
    const size_t featureCount = sourceLayer->featureCount();
    for (size_t i = 0; i < featureCount; ++i) {
        auto feature = sourceLayer->getFeature(i);
        if (!leader.filter(feature->getType(), feature->getID(), [&] (const auto& key) { return feature->getValue(key); }))
            continue;
        
        SymbolFeature ft(std::move(feature));

        ft.index = i;

        auto getValue = [&ft](const std::string& key) -> std::string {
            auto value = ft.getValue(key);
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
            std::string u8string = layout.evaluate<TextField>(zoom, ft);
            if (layout.get<TextField>().isConstant()) {
                u8string = util::replaceTokens(u8string, getValue);
            }
            
            auto textTransform = layout.evaluate<TextTransform>(zoom, ft);

            if (textTransform == TextTransformType::Uppercase) {
                u8string = platform::uppercase(u8string);
            } else if (textTransform == TextTransformType::Lowercase) {
                u8string = platform::lowercase(u8string);
            }

            ft.text = applyArabicShaping(util::utf8_to_utf16::convert(u8string));
            const bool canVerticalizeText = layout.get<TextRotationAlignment>() == AlignmentType::Map
                                         && layout.get<SymbolPlacement>() == SymbolPlacementType::Line
                                         && util::i18n::allowsVerticalWritingMode(*ft.text);

            // Loop through all characters of this text and collect unique codepoints.
            for (char16_t chr : *ft.text) {
                glyphDependencies[layout.get<TextFont>()].insert(chr);
                if (canVerticalizeText) {
                    if (char16_t verticalChr = util::i18n::verticalizePunctuation(chr)) {
                        glyphDependencies[layout.get<TextFont>()].insert(verticalChr);
                    }
                }
            }
        }

        if (hasIcon) {
            std::string icon = layout.evaluate<IconImage>(zoom, ft);
            if (layout.get<IconImage>().isConstant()) {
                icon = util::replaceTokens(icon, getValue);
            }
            ft.icon = icon;
            imageDependencies.insert(*ft.icon);
        }

        if (ft.text || ft.icon) {
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

void SymbolLayout::prepare(const GlyphMap& glyphMap, const GlyphPositions& glyphPositions,
                           const ImageMap& imageMap, const ImagePositions& imagePositions) {
    const bool textAlongLine = layout.get<TextRotationAlignment>() == AlignmentType::Map &&
        layout.get<SymbolPlacement>() == SymbolPlacementType::Line;

    auto glyphMapIt = glyphMap.find(layout.get<TextFont>());
    const Glyphs& glyphs = glyphMapIt != glyphMap.end()
        ? glyphMapIt->second : Glyphs();

    auto glyphPositionsIt = glyphPositions.find(layout.get<TextFont>());
    const GlyphPositionMap& glyphPositionMap = glyphPositionsIt != glyphPositions.end()
        ? glyphPositionsIt->second : GlyphPositionMap();

    for (auto it = features.begin(); it != features.end(); ++it) {
        auto& feature = *it;
        if (feature.geometry.empty()) continue;

        std::pair<Shaping, Shaping> shapedTextOrientations;
        optional<PositionedIcon> shapedIcon;

        // if feature has text, shape the text
        if (feature.text) {
            auto applyShaping = [&] (const std::u16string& text, WritingModeType writingMode) {
                const float oneEm = 24.0f;
                const Shaping result = getShaping(
                    /* string */ text,
                    /* maxWidth: ems */ layout.get<SymbolPlacement>() != SymbolPlacementType::Line ?
                        layout.get<TextMaxWidth>() * oneEm : 0,
                    /* lineHeight: ems */ layout.get<TextLineHeight>() * oneEm,
                    /* anchor */ layout.evaluate<TextAnchor>(zoom, feature),
                    /* justify */ layout.evaluate<TextJustify>(zoom, feature),
                    /* spacing: ems */ util::i18n::allowsLetterSpacing(*feature.text) ? layout.get<TextLetterSpacing>() * oneEm : 0.0f,
                    /* translate */ Point<float>(layout.evaluate<TextOffset>(zoom, feature)[0] * oneEm, layout.evaluate<TextOffset>(zoom, feature)[1] * oneEm),
                    /* verticalHeight */ oneEm,
                    /* writingMode */ writingMode,
                    /* bidirectional algorithm object */ bidi,
                    /* glyphs */ glyphs);

                return result;
            };

            shapedTextOrientations.first = applyShaping(*feature.text, WritingModeType::Horizontal);

            if (util::i18n::allowsVerticalWritingMode(*feature.text) && textAlongLine) {
                shapedTextOrientations.second = applyShaping(util::i18n::verticalizePunctuation(*feature.text), WritingModeType::Vertical);
            }
        }

        // if feature has icon, get sprite atlas position
        if (feature.icon) {
            auto image = imageMap.find(*feature.icon);
            if (image != imageMap.end()) {
                shapedIcon = PositionedIcon::shapeIcon(
                    imagePositions.at(*feature.icon),
                    layout.evaluate<IconOffset>(zoom, feature),
                    layout.evaluate<IconAnchor>(zoom, feature),
                    layout.evaluate<IconRotate>(zoom, feature) * util::DEG2RAD);
                if (image->second->sdf) {
                    sdfIcons = true;
                }
                if (image->second->pixelRatio != pixelRatio) {
                    iconsNeedLinear = true;
                } else if (layout.get<IconRotate>().constantOr(1) != 0) {
                    iconsNeedLinear = true;
                }
            }
        }

        // if either shapedText or icon position is present, add the feature
        if (shapedTextOrientations.first || shapedIcon) {
            addFeature(std::distance(features.begin(), it), feature, shapedTextOrientations, shapedIcon, glyphPositionMap);
        }
        
        feature.geometry.clear();
    }

    compareText.clear();
}

void SymbolLayout::addFeature(const std::size_t index,
                              const SymbolFeature& feature,
                              const std::pair<Shaping, Shaping>& shapedTextOrientations,
                              optional<PositionedIcon> shapedIcon,
                              const GlyphPositionMap& glyphPositionMap) {
    const float minScale = 0.5f;
    const float glyphSize = 24.0f;
    
    const float layoutTextSize = layout.evaluate<TextSize>(zoom + 1, feature);
    const float layoutIconSize = layout.evaluate<IconSize>(zoom + 1, feature);
    const std::array<float, 2> textOffset = layout.evaluate<TextOffset>(zoom, feature);
    const std::array<float, 2> iconOffset = layout.evaluate<IconOffset>(zoom, feature);
    
    // To reduce the number of labels that jump around when zooming we need
    // to use a text-size value that is the same for all zoom levels.
    // This calculates text-size at a high zoom level so that all tiles can
    // use the same value when calculating anchor positions.
    const float textMaxSize = layout.evaluate<TextSize>(18, feature);
    
    const float fontScale = layoutTextSize / glyphSize;
    const float textBoxScale = tilePixelRatio * fontScale;
    const float textMaxBoxScale = tilePixelRatio * textMaxSize / glyphSize;
    const float iconBoxScale = tilePixelRatio * layoutIconSize;
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
    IndexedSubfeature indexedFeature = { feature.index, sourceLayer->getName(), bucketName,
                                         symbolInstances.size() };

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

        symbolInstances.emplace_back(anchor, line, shapedTextOrientations, shapedIcon,
                layout.evaluate(zoom, feature), layoutTextSize,
                addToBuffers, symbolInstances.size(),
                textBoxScale, textPadding, textPlacement, textOffset,
                iconBoxScale, iconPadding, iconPlacement, iconOffset,
                glyphPositionMap, indexedFeature, index);
    };
    
    const auto& type = feature.getType();

    if (layout.get<SymbolPlacement>() == SymbolPlacementType::Line) {
        auto clippedLines = util::clipLines(feature.geometry, 0, 0, util::EXTENT, util::EXTENT);
        for (const auto& line : clippedLines) {
            Anchors anchors = getAnchors(line,
                                         symbolSpacing,
                                         textMaxAngle,
                                         (shapedTextOrientations.second ?: shapedTextOrientations.first).left,
                                         (shapedTextOrientations.second ?: shapedTextOrientations.first).right,
                                         (shapedIcon ? shapedIcon->left() : 0),
                                         (shapedIcon ? shapedIcon->right() : 0),
                                         glyphSize,
                                         textMaxBoxScale,
                                         overscaling);

            for (auto& anchor : anchors) {
                if (!feature.text || !anchorIsTooClose(*feature.text, textRepeatDistance, anchor)) {
                    addSymbolInstance(line, anchor);
                }
            }
        }
    } else if (type == FeatureType::Polygon) {
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
    } else if (type == FeatureType::LineString) {
        for (const auto& line : feature.geometry) {
            Anchor anchor(line[0].x, line[0].y, 0, minScale);
            addSymbolInstance(line, anchor);
        }
    } else if (type == FeatureType::Point) {
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
    auto bucket = std::make_unique<SymbolBucket>(layout, layerPaintProperties, textSize, iconSize, zoom, sdfIcons, iconsNeedLinear);

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

    const bool keepUpright = layout.get<TextKeepUpright>();

    // Sort symbols by their y position on the canvas so that they lower symbols
    // are drawn on top of higher symbols.
    // Don't sort symbols that won't overlap because it isn't necessary and
    // because it causes more labels to pop in and out when rotating.
    if (mayOverlap) {
        const float sin = std::sin(collisionTile.config.angle);
        const float cos = std::cos(collisionTile.config.angle);

        std::sort(symbolInstances.begin(), symbolInstances.end(), [sin, cos](SymbolInstance &a, SymbolInstance &b) {
            const int32_t aRotated = sin * a.anchor.point.x + cos * a.anchor.point.y;
            const int32_t bRotated = sin * b.anchor.point.x + cos * b.anchor.point.y;
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

        const auto& feature = features.at(symbolInstance.featureIndex);

        // Insert final placement into collision tree and add glyphs/icons to buffers

        if (hasText) {
            const float placementZoom = util::max(util::log2(glyphScale) + zoom, 0.0f);
            collisionTile.insertFeature(symbolInstance.textCollisionFeature, glyphScale, layout.get<TextIgnorePlacement>());
            if (glyphScale < collisionTile.maxScale) {

                const float labelAngle = std::fmod((symbolInstance.anchor.angle + collisionTile.config.angle) + 2 * M_PI, 2 * M_PI);
                const bool inVerticalRange = (
                    (labelAngle > M_PI * 1.0 / 4.0 && labelAngle <= M_PI * 3.0 / 4) ||
                    (labelAngle > M_PI * 5.0 / 4.0 && labelAngle <= M_PI * 7.0 / 4));
                const bool useVerticalMode = symbolInstance.writingModes & WritingModeType::Vertical && inVerticalRange;

                const Range<float> sizeData = bucket->textSizeBinder->getVertexSizeData(feature);
                bucket->text.placedSymbols.emplace_back(symbolInstance.anchor.point, symbolInstance.anchor.segment, sizeData.min, sizeData.max,
                        symbolInstance.textOffset, placementZoom, useVerticalMode, symbolInstance.line);

                for (const auto& symbol : symbolInstance.glyphQuads) {
                    addSymbol(
                        bucket->text, sizeData, symbol, placementZoom,
                        keepUpright, textPlacement, symbolInstance.anchor, bucket->text.placedSymbols.back());
                }
            }
        }

        if (hasIcon) {
            const float placementZoom = util::max(util::log2(iconScale) + zoom, 0.0f);
            collisionTile.insertFeature(symbolInstance.iconCollisionFeature, iconScale, layout.get<IconIgnorePlacement>());
            if (iconScale < collisionTile.maxScale && symbolInstance.iconQuad) {
                const Range<float> sizeData = bucket->iconSizeBinder->getVertexSizeData(feature);
                bucket->icon.placedSymbols.emplace_back(symbolInstance.anchor.point, symbolInstance.anchor.segment, sizeData.min, sizeData.max,
                        symbolInstance.iconOffset, placementZoom, false, symbolInstance.line);
                addSymbol(
                    bucket->icon, sizeData, *symbolInstance.iconQuad, placementZoom,
                    keepUpright, iconPlacement, symbolInstance.anchor, bucket->icon.placedSymbols.back());
            }
        }
        
        for (auto& pair : bucket->paintPropertyBinders) {
            pair.second.first.populateVertexVectors(feature, bucket->icon.vertices.vertexSize());
            pair.second.second.populateVertexVectors(feature, bucket->text.vertices.vertexSize());
        }
    }

    if (collisionTile.config.debug) {
        addToDebugBuffers(collisionTile, *bucket);
    }

    return bucket;
}

template <typename Buffer>
void SymbolLayout::addSymbol(Buffer& buffer,
                             const Range<float> sizeData,
                             const SymbolQuad& symbol,
                             const float placementZoom,
                             const bool keepUpright,
                             const style::SymbolPlacementType placement,
                             const Anchor& labelAnchor,
                             PlacedSymbol& placedSymbol) {
    constexpr const uint16_t vertexLength = 4;

    const auto &tl = symbol.tl;
    const auto &tr = symbol.tr;
    const auto &bl = symbol.bl;
    const auto &br = symbol.br;
    const auto &tex = symbol.tex;

    if (placement == style::SymbolPlacementType::Line && keepUpright) {
        // drop incorrectly oriented glyphs
        if ((symbol.writingMode == WritingModeType::Vertical) != placedSymbol.useVerticalMode) return;
    }

    if (buffer.segments.empty() || buffer.segments.back().vertexLength + vertexLength > std::numeric_limits<uint16_t>::max()) {
        buffer.segments.emplace_back(buffer.vertices.vertexSize(), buffer.triangles.indexSize());
    }

    // We're generating triangle fans, so we always start with the first
    // coordinate in this polygon.
    auto& segment = buffer.segments.back();
    assert(segment.vertexLength <= std::numeric_limits<uint16_t>::max());
    uint16_t index = segment.vertexLength;

    // coordinates (2 triangles)
    buffer.vertices.emplace_back(SymbolLayoutAttributes::vertex(labelAnchor.point, tl, symbol.glyphOffset.y, tex.x, tex.y, sizeData));
    buffer.vertices.emplace_back(SymbolLayoutAttributes::vertex(labelAnchor.point, tr, symbol.glyphOffset.y, tex.x + tex.w, tex.y, sizeData));
    buffer.vertices.emplace_back(SymbolLayoutAttributes::vertex(labelAnchor.point, bl, symbol.glyphOffset.y, tex.x, tex.y + tex.h, sizeData));
    buffer.vertices.emplace_back(SymbolLayoutAttributes::vertex(labelAnchor.point, br, symbol.glyphOffset.y, tex.x + tex.w, tex.y + tex.h, sizeData));
    
    auto dynamicVertex = SymbolDynamicLayoutAttributes::vertex(labelAnchor.point, 0, placementZoom);
    buffer.dynamicVertices.emplace_back(dynamicVertex);
    buffer.dynamicVertices.emplace_back(dynamicVertex);
    buffer.dynamicVertices.emplace_back(dynamicVertex);
    buffer.dynamicVertices.emplace_back(dynamicVertex);

    // add the two triangles, referencing the four coordinates we just inserted.
    buffer.triangles.emplace_back(index + 0, index + 1, index + 2);
    buffer.triangles.emplace_back(index + 1, index + 2, index + 3);

    segment.vertexLength += vertexLength;
    segment.indexLength += 6;

    placedSymbol.glyphOffsets.push_back(symbol.glyphOffset.x);
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

                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, symbolInstance.anchor.point, tl, maxZoom, placementZoom));
                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, symbolInstance.anchor.point, tr, maxZoom, placementZoom));
                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, symbolInstance.anchor.point, br, maxZoom, placementZoom));
                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, symbolInstance.anchor.point, bl, maxZoom, placementZoom));

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
