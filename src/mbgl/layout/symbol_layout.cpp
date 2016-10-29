#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/layout/merge_lines.hpp>
#include <mbgl/layout/clip_lines.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/style/filter_evaluator.hpp>
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
#include <mbgl/math/clamp.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/log.hpp>

namespace mbgl {

using namespace style;

SymbolLayout::SymbolLayout(std::string bucketName_,
                           std::string sourceLayerName_,
                           uint32_t overscaling_,
                           float zoom_,
                           const MapMode mode_,
                           const GeometryTileLayer& layer,
                           const style::Filter& filter,
                           style::SymbolLayoutProperties layout_,
                           float textMaxSize_,
                           SpriteAtlas& spriteAtlas_)
    : bucketName(std::move(bucketName_)),
      sourceLayerName(std::move(sourceLayerName_)),
      overscaling(overscaling_),
      zoom(zoom_),
      mode(mode_),
      layout(std::move(layout_)),
      textMaxSize(textMaxSize_),
      spriteAtlas(spriteAtlas_),
      tileSize(util::tileSize * overscaling_),
      tilePixelRatio(float(util::EXTENT) / tileSize) {

    const bool hasText = !layout.textField.value.empty() && !layout.textFont.value.empty();
    const bool hasIcon = !layout.iconImage.value.empty();

    if (!hasText && !hasIcon) {
        return;
    }

    auto layerName = layer.getName();

    // Determine and load glyph ranges
    const size_t featureCount = layer.featureCount();
    for (size_t i = 0; i < featureCount; ++i) {
        auto feature = layer.getFeature(i);
        if (!filter(feature->getType(), feature->getID(), [&] (const auto& key) { return feature->getValue(key); }))
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
            std::string u8string = util::replaceTokens(layout.textField, getValue);

            if (layout.textTransform == TextTransformType::Uppercase) {
                u8string = platform::uppercase(u8string);
            } else if (layout.textTransform == TextTransformType::Lowercase) {
                u8string = platform::lowercase(u8string);
            }

            ft.text = util::utf8_to_utf32::convert(u8string);

            // Loop through all characters of this text and collect unique codepoints.
            for (char32_t chr : *ft.text) {
                ranges.insert(getGlyphRange(chr));
            }
        }

        if (hasIcon) {
            ft.icon = util::replaceTokens(layout.iconImage, getValue);
        }

        if (ft.text || ft.icon) {
            auto &multiline = ft.geometry;

            GeometryCollection geometryCollection = feature->getGeometries();
            for (auto& line : geometryCollection) {
                multiline.emplace_back();
                for (auto& point : line) {
                    multiline.back().emplace_back(point.x, point.y);
                }
            }

            features.push_back(std::move(ft));
        }
    }

    if (layout.symbolPlacement == SymbolPlacementType::Line) {
        util::mergeLines(features);
    }
}

bool SymbolLayout::hasSymbolInstances() const {
    return !symbolInstances.empty();
}

bool SymbolLayout::canPrepare(GlyphAtlas& glyphAtlas) {
    if (!layout.textField.value.empty() && !layout.textFont.value.empty() && !glyphAtlas.hasGlyphRanges(layout.textFont, ranges)) {
        return false;
    }

    if (!layout.iconImage.value.empty() && !spriteAtlas.isLoaded()) {
        return false;
    }

    return true;
}

void SymbolLayout::prepare(uintptr_t tileUID,
                           GlyphAtlas& glyphAtlas) {
    float horizontalAlign = 0.5;
    float verticalAlign = 0.5;

    switch (layout.textAnchor) {
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

    switch (layout.textAnchor) {
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

    const float justify = layout.textJustify == TextJustifyType::Right ? 1 :
        layout.textJustify == TextJustifyType::Left ? 0 :
        0.5;

    auto glyphSet = glyphAtlas.getGlyphSet(layout.textFont);

    for (const auto& feature : features) {
        if (feature.geometry.empty()) continue;

        Shaping shapedText;
        PositionedIcon shapedIcon;
        GlyphPositions face;

        // if feature has text, shape the text
        if (feature.text) {
            shapedText = glyphSet->getShaping(
                /* string */ *feature.text,
                /* maxWidth: ems */ layout.symbolPlacement != SymbolPlacementType::Line ?
                    layout.textMaxWidth * 24 : 0,
                /* lineHeight: ems */ layout.textLineHeight * 24,
                /* horizontalAlign */ horizontalAlign,
                /* verticalAlign */ verticalAlign,
                /* justify */ justify,
                /* spacing: ems */ layout.textLetterSpacing * 24,
                /* translate */ Point<float>(layout.textOffset.value[0], layout.textOffset.value[1]));

            // Add the glyphs we need for this label to the glyph atlas.
            if (shapedText) {
                glyphAtlas.addGlyphs(tileUID, *feature.text, layout.textFont, **glyphSet, face);
            }
        }

        // if feature has icon, get sprite atlas position
        if (feature.icon) {
            auto image = spriteAtlas.getImage(*feature.icon, SpritePatternMode::Single);
            if (image) {
                shapedIcon = shapeIcon(*image, layout);
                assert((*image).spriteImage);
                if ((*image).spriteImage->sdf) {
                    sdfIcons = true;
                }
                if ((*image).relativePixelRatio != 1.0f) {
                    iconsNeedLinear = true;
                } else if (layout.iconRotate != 0) {
                    iconsNeedLinear = true;
                }
            }
        }

        // if either shapedText or icon position is present, add the feature
        if (shapedText || shapedIcon) {
            addFeature(feature.geometry, shapedText, shapedIcon, face, feature.index);
        }
    }

    features.clear();
}


void SymbolLayout::addFeature(const GeometryCollection &lines,
        const Shaping &shapedText, const PositionedIcon &shapedIcon, const GlyphPositions &face, const size_t index) {

    const float minScale = 0.5f;
    const float glyphSize = 24.0f;

    const float fontScale = layout.textSize / glyphSize;
    const float textBoxScale = tilePixelRatio * fontScale;
    const float textMaxBoxScale = tilePixelRatio * textMaxSize / glyphSize;
    const float iconBoxScale = tilePixelRatio * layout.iconSize;
    const float symbolSpacing = tilePixelRatio * layout.symbolSpacing;
    const bool avoidEdges = layout.symbolAvoidEdges && layout.symbolPlacement != SymbolPlacementType::Line;
    const float textPadding = layout.textPadding * tilePixelRatio;
    const float iconPadding = layout.iconPadding * tilePixelRatio;
    const float textMaxAngle = layout.textMaxAngle * util::DEG2RAD;
    const SymbolPlacementType textPlacement = layout.textRotationAlignment != AlignmentType::Map
                                                  ? SymbolPlacementType::Point
                                                  : layout.symbolPlacement;
    const SymbolPlacementType iconPlacement = layout.iconRotationAlignment != AlignmentType::Map
                                                  ? SymbolPlacementType::Point
                                                  : layout.symbolPlacement;
    const bool mayOverlap = layout.textAllowOverlap || layout.iconAllowOverlap ||
        layout.textIgnorePlacement || layout.iconIgnorePlacement;
    const bool isLine = layout.symbolPlacement == SymbolPlacementType::Line;
    const float textRepeatDistance = symbolSpacing / 2;

    auto& clippedLines = isLine ?
        util::clipLines(lines, 0, 0, util::EXTENT, util::EXTENT) :
        lines;

    IndexedSubfeature indexedFeature = {index, sourceLayerName, bucketName, symbolInstances.size()};

    for (const auto& line : clippedLines) {
        if (line.empty()) continue;

        // Calculate the anchor points around which you want to place labels
        Anchors anchors = isLine ?
            getAnchors(line, symbolSpacing, textMaxAngle, shapedText.left, shapedText.right, shapedIcon.left, shapedIcon.right, glyphSize, textMaxBoxScale, overscaling) :
            Anchors({ Anchor(float(line[0].x), float(line[0].y), 0, minScale) });

        // For each potential label, create the placement features used to check for collisions, and the quads use for rendering.
        for (Anchor &anchor : anchors) {
            if (shapedText && isLine) {
                if (anchorIsTooClose(shapedText.text, textRepeatDistance, anchor)) {
                    continue;
                }
            }

            const bool inside = !(anchor.point.x < 0 || anchor.point.x > util::EXTENT || anchor.point.y < 0 || anchor.point.y > util::EXTENT);

            if (avoidEdges && !inside) continue;

            // Normally symbol layers are drawn across tile boundaries. Only symbols
            // with their anchors within the tile boundaries are added to the buffers
            // to prevent symbols from being drawn twice.
            //
            // Symbols in layers with overlap are sorted in the y direction so that
            // symbols lower on the canvas are drawn on top of symbols near the top.
            // To preserve this order across tile boundaries these symbols can't
            // be drawn across tile boundaries. Instead they need to be included in
            // the buffers for both tiles and clipped to tile boundaries at draw time.
            //
            // TODO remove the `&& false` when is #1673 implemented
            const bool addToBuffers = (mode == MapMode::Still) || inside || (mayOverlap && false);

            symbolInstances.emplace_back(anchor, line, shapedText, shapedIcon, layout, addToBuffers, symbolInstances.size(),
                    textBoxScale, textPadding, textPlacement,
                    iconBoxScale, iconPadding, iconPlacement,
                    face, indexedFeature);
        }
    }
}

bool SymbolLayout::anchorIsTooClose(const std::u32string &text, const float repeatDistance, Anchor &anchor) {
    if (compareText.find(text) == compareText.end()) {
        compareText.emplace(text, Anchors());
    } else {
        auto otherAnchors = compareText.find(text)->second;
        for (Anchor &otherAnchor : otherAnchors) {
            if (util::dist<float>(anchor.point, otherAnchor.point) < repeatDistance) {
                return true;
            }
        }
    }
    compareText[text].push_back(anchor);
    return false;
}

std::unique_ptr<SymbolBucket> SymbolLayout::place(CollisionTile& collisionTile) {
    auto bucket = std::make_unique<SymbolBucket>(mode, layout, sdfIcons, iconsNeedLinear);

    // Calculate which labels can be shown and when they can be shown and
    // create the bufers used for rendering.

    const SymbolPlacementType textPlacement = layout.textRotationAlignment != AlignmentType::Map
                                                  ? SymbolPlacementType::Point
                                                  : layout.symbolPlacement;
    const SymbolPlacementType iconPlacement = layout.iconRotationAlignment != AlignmentType::Map
                                                  ? SymbolPlacementType::Point
                                                  : layout.symbolPlacement;

    const bool mayOverlap = layout.textAllowOverlap || layout.iconAllowOverlap ||
        layout.textIgnorePlacement || layout.iconIgnorePlacement;

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

        const bool iconWithoutText = layout.textOptional || !hasText;
        const bool textWithoutIcon = layout.iconOptional || !hasIcon;

        // Calculate the scales at which the text and icon can be placed without collision.

        float glyphScale = hasText ?
            collisionTile.placeFeature(symbolInstance.textCollisionFeature,
                    layout.textAllowOverlap, layout.symbolAvoidEdges) :
            collisionTile.minScale;
        float iconScale = hasIcon ?
            collisionTile.placeFeature(symbolInstance.iconCollisionFeature,
                    layout.iconAllowOverlap, layout.symbolAvoidEdges) :
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
            collisionTile.insertFeature(symbolInstance.textCollisionFeature, glyphScale, layout.textIgnorePlacement);
            if (glyphScale < collisionTile.maxScale) {
                addSymbols(
                    bucket->text, symbolInstance.glyphQuads, glyphScale,
                    layout.textKeepUpright, textPlacement, collisionTile.config.angle);
            }
        }

        if (hasIcon) {
            collisionTile.insertFeature(symbolInstance.iconCollisionFeature, iconScale, layout.iconIgnorePlacement);
            if (iconScale < collisionTile.maxScale) {
                addSymbols(
                    bucket->icon, symbolInstance.iconQuads, iconScale,
                    layout.iconKeepUpright, iconPlacement, collisionTile.config.angle);
            }
        }
    }

    if (collisionTile.config.debug) {
        addToDebugBuffers(collisionTile, *bucket);
    }

    return bucket;
}

template <typename Buffer>
void SymbolLayout::addSymbols(Buffer &buffer, const SymbolQuads &symbols, float scale, const bool keepUpright, const style::SymbolPlacementType placement, const float placementAngle) {
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

        // drop upside down versions of glyphs
        const float a = std::fmod(symbol.anchorAngle + placementAngle + M_PI, M_PI * 2);
        if (keepUpright && placement == style::SymbolPlacementType::Line &&
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
            buffer.segments.emplace_back(buffer.vertices.size(), buffer.triangles.size());
        }

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        auto& segment = buffer.segments.back();
        assert(segment.vertexLength <= std::numeric_limits<uint16_t>::max());
        uint16_t index = segment.vertexLength;

        // Encode angle of glyph
        uint8_t glyphAngle = std::round((symbol.glyphAngle / (M_PI * 2)) * 256);

        // coordinates (2 triangles)
        buffer.vertices.emplace_back(SymbolAttributes::vertex(anchorPoint, tl, tex.x, tex.y,
                            minZoom, maxZoom, placementZoom, glyphAngle));
        buffer.vertices.emplace_back(SymbolAttributes::vertex(anchorPoint, tr, tex.x + tex.w, tex.y,
                            minZoom, maxZoom, placementZoom, glyphAngle));
        buffer.vertices.emplace_back(SymbolAttributes::vertex(anchorPoint, bl, tex.x, tex.y + tex.h,
                            minZoom, maxZoom, placementZoom, glyphAngle));
        buffer.vertices.emplace_back(SymbolAttributes::vertex(anchorPoint, br, tex.x + tex.w, tex.y + tex.h,
                            minZoom, maxZoom, placementZoom, glyphAngle));

        // add the two triangles, referencing the four coordinates we just inserted.
        buffer.triangles.emplace_back(index + 0, index + 1, index + 2);
        buffer.triangles.emplace_back(index + 1, index + 2, index + 3);

        segment.vertexLength += vertexLength;
        segment.primitiveLength += 2;
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

                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, tl, maxZoom, placementZoom));
                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, tr, maxZoom, placementZoom));
                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, tr, maxZoom, placementZoom));
                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, br, maxZoom, placementZoom));
                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, br, maxZoom, placementZoom));
                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, bl, maxZoom, placementZoom));
                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, bl, maxZoom, placementZoom));
                collisionBox.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, tl, maxZoom, placementZoom));
            }
        };
        populateCollisionBox(symbolInstance.textCollisionFeature);
        populateCollisionBox(symbolInstance.iconCollisionFeature);
    }
}

} // namespace mbgl
