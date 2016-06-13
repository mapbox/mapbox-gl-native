#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/sprite/sprite_store.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/geometry/text_buffer.hpp>
#include <mbgl/geometry/icon_buffer.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/anchor.hpp>
#include <mbgl/text/get_anchors.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/text/glyph_set.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/shader/sdf_shader.hpp>
#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/shader/collision_box_shader.hpp>

#include <mbgl/util/utf.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/merge_lines.hpp>
#include <mbgl/util/clip_lines.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/math/minmax.hpp>

namespace mbgl {

using namespace style;

SymbolInstance::SymbolInstance(Anchor& anchor, const GeometryCoordinates& line,
        const Shaping& shapedText, const PositionedIcon& shapedIcon,
        const SymbolLayoutProperties& layout, const bool addToBuffers, const uint32_t index_,
        const float textBoxScale, const float textPadding, const float textAlongLine,
        const float iconBoxScale, const float iconPadding, const float iconAlongLine,
        const GlyphPositions& face, const IndexedSubfeature& indexedFeature) :
    point(anchor.point),
    index(index_),
    hasText(shapedText),
    hasIcon(shapedIcon),

    // Create the quads used for rendering the glyphs.
    glyphQuads(addToBuffers && shapedText ?
            getGlyphQuads(anchor, shapedText, textBoxScale, line, layout, textAlongLine, face) :
            SymbolQuads()),

    // Create the quad used for rendering the icon.
    iconQuads(addToBuffers && shapedIcon ?
            getIconQuads(anchor, shapedIcon, line, layout, iconAlongLine, shapedText) :
            SymbolQuads()),

    // Create the collision features that will be used to check whether this symbol instance can be placed
    textCollisionFeature(line, anchor, shapedText, textBoxScale, textPadding, textAlongLine, indexedFeature),
    iconCollisionFeature(line, anchor, shapedIcon, iconBoxScale, iconPadding, iconAlongLine, indexedFeature)
    {}


SymbolBucket::SymbolBucket(uint32_t overscaling_, float zoom_, const MapMode mode_, std::string bucketName_, std::string sourceLayerName_)
    : overscaling(overscaling_),
      zoom(zoom_),
      tileSize(util::tileSize * overscaling_),
      tilePixelRatio(float(util::EXTENT) / tileSize),
      mode(mode_),
      bucketName(std::move(bucketName_)),
      sourceLayerName(std::move(sourceLayerName_)) {}

SymbolBucket::~SymbolBucket() {
    // Do not remove. header file only contains forward definitions to unique pointers.
}

void SymbolBucket::upload(gl::ObjectStore& store) {
    if (hasTextData()) {
        renderData->text.vertices.upload(store);
        renderData->text.triangles.upload(store);
    }
    if (hasIconData()) {
        renderData->icon.vertices.upload(store);
        renderData->icon.triangles.upload(store);
    }

    uploaded = true;
}

void SymbolBucket::render(Painter& painter,
                          const Layer& layer,
                          const UnwrappedTileID& tileID,
                          const mat4& matrix) {
    painter.renderSymbol(*this, *layer.as<SymbolLayer>(), tileID, matrix);
}

bool SymbolBucket::hasData() const { return hasTextData() || hasIconData() || !symbolInstances.empty(); }

bool SymbolBucket::hasTextData() const { return renderData && !renderData->text.groups.empty(); }

bool SymbolBucket::hasIconData() const { return renderData && !renderData->icon.groups.empty(); }

bool SymbolBucket::hasCollisionBoxData() const { return renderData && !renderData->collisionBox.groups.empty(); }

bool SymbolBucket::needsClipping() const {
    return mode == MapMode::Still;
}

void SymbolBucket::parseFeatures(const GeometryTileLayer& layer, const Filter& filter) {
    const bool has_text = !layout.textField.value.empty() && !layout.textFont.value.empty();
    const bool has_icon = !layout.iconImage.value.empty();

    if (!has_text && !has_icon) {
        return;
    }

    auto layerName = layer.getName();

    // Determine and load glyph ranges
    const GLsizei featureCount = static_cast<GLsizei>(layer.featureCount());
    for (GLsizei i = 0; i < featureCount; i++) {
        auto feature = layer.getFeature(i);

        FilterEvaluator evaluator(*feature);
        if (!Filter::visit(filter, evaluator))
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

        if (has_text) {
            std::string u8string = util::replaceTokens(layout.textField, getValue);

            if (layout.textTransform == TextTransformType::Uppercase) {
                u8string = platform::uppercase(u8string);
            } else if (layout.textTransform == TextTransformType::Lowercase) {
                u8string = platform::lowercase(u8string);
            }

            ft.label = util::utf8_to_utf32::convert(u8string);

            if (!ft.label.empty()) {
                // Loop through all characters of this text and collect unique codepoints.
                for (char32_t chr : ft.label) {
                    ranges.insert(getGlyphRange(chr));
                }
            }
        }

        if (has_icon) {
            ft.sprite = util::replaceTokens(layout.iconImage, getValue);
        }

        if (ft.label.length() || ft.sprite.length()) {

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

bool SymbolBucket::needsDependencies(GlyphStore& glyphStore, SpriteStore& spriteStore) {
    if (!layout.textField.value.empty() && !layout.textFont.value.empty() && !glyphStore.hasGlyphRanges(layout.textFont, ranges)) {
        return true;
    }

    if (!layout.iconImage.value.empty() && !spriteStore.isLoaded()) {
        return true;
    }

    return false;
}

void SymbolBucket::addFeatures(uintptr_t tileUID,
                               SpriteAtlas& spriteAtlas,
                               GlyphAtlas& glyphAtlas,
                               GlyphStore& glyphStore) {
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

    auto glyphSet = glyphStore.getGlyphSet(layout.textFont);

    for (const auto& feature : features) {
        if (feature.geometry.empty()) continue;

        Shaping shapedText;
        PositionedIcon shapedIcon;
        GlyphPositions face;

        // if feature has text, shape the text
        if (feature.label.length()) {
            shapedText = glyphSet->getShaping(
                /* string */ feature.label,
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
                glyphAtlas.addGlyphs(tileUID, feature.label, layout.textFont, **glyphSet, face);
            }
        }

        // if feature has icon, get sprite atlas position
        if (feature.sprite.length()) {
            auto image = spriteAtlas.getImage(feature.sprite, false);
            if (image) {
                shapedIcon = shapeIcon(*image, layout);
                assert((*image).spriteImage);
                if ((*image).spriteImage->sdf) {
                    sdfIcons = true;
                }
                if ((*image).relativePixelRatio != 1.0f) {
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


void SymbolBucket::addFeature(const GeometryCollection &lines,
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
    const bool textAlongLine =
        layout.textRotationAlignment == AlignmentType::Map &&
        layout.symbolPlacement == SymbolPlacementType::Line;
    const bool iconAlongLine =
        layout.iconRotationAlignment == AlignmentType::Map &&
        layout.symbolPlacement == SymbolPlacementType::Line;
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
                    textBoxScale, textPadding, textAlongLine,
                    iconBoxScale, iconPadding, iconAlongLine,
                    face, indexedFeature);
        }
    }
}
    
bool SymbolBucket::anchorIsTooClose(const std::u32string &text, const float repeatDistance, Anchor &anchor) {
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

void SymbolBucket::placeFeatures(CollisionTile& collisionTile) {

    renderDataInProgress = std::make_unique<SymbolRenderData>();

    // Calculate which labels can be shown and when they can be shown and
    // create the bufers used for rendering.

    const bool textAlongLine =
        layout.textRotationAlignment == AlignmentType::Map &&
        layout.symbolPlacement == SymbolPlacementType::Line;
    const bool iconAlongLine =
        layout.iconRotationAlignment == AlignmentType::Map &&
        layout.symbolPlacement == SymbolPlacementType::Line;

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
                addSymbols<SymbolRenderData::TextBuffer, TextElementGroup>(
                    renderDataInProgress->text, symbolInstance.glyphQuads, glyphScale,
                    layout.textKeepUpright, textAlongLine, collisionTile.config.angle);
            }
        }

        if (hasIcon) {
            collisionTile.insertFeature(symbolInstance.iconCollisionFeature, iconScale, layout.iconIgnorePlacement);
            if (iconScale < collisionTile.maxScale) {
                addSymbols<SymbolRenderData::IconBuffer, IconElementGroup>(
                    renderDataInProgress->icon, symbolInstance.iconQuads, iconScale,
                    layout.iconKeepUpright, iconAlongLine, collisionTile.config.angle);
            }
        }
    }

    if (collisionTile.config.debug) {
        addToDebugBuffers(collisionTile);
    }
}

template <typename Buffer, typename GroupType>
void SymbolBucket::addSymbols(Buffer &buffer, const SymbolQuads &symbols, float scale, const bool keepUpright, const bool alongLine, const float placementAngle) {

    const float placementZoom = ::fmax(std::log(scale) / std::log(2) + zoom, 0);

    for (const auto& symbol : symbols) {
        const auto &tl = symbol.tl;
        const auto &tr = symbol.tr;
        const auto &bl = symbol.bl;
        const auto &br = symbol.br;
        const auto &tex = symbol.tex;

        float minZoom =
            util::max(static_cast<float>(zoom + log(symbol.minScale) / log(2)), placementZoom);
        float maxZoom = util::min(static_cast<float>(zoom + log(symbol.maxScale) / log(2)), 25.0f);
        const auto &anchorPoint = symbol.anchorPoint;

        // drop upside down versions of glyphs
        const float a = std::fmod(symbol.anchorAngle + placementAngle + M_PI, M_PI * 2);
        if (keepUpright && alongLine && (a <= M_PI / 2 || a > M_PI * 3 / 2)) continue;


        if (maxZoom <= minZoom)
            continue;

        // Lower min zoom so that while fading out the label
        // it can be shown outside of collision-free zoom levels
        if (minZoom == placementZoom) {
            minZoom = 0;
        }

        const int glyph_vertex_length = 4;

        if (buffer.groups.empty() || (buffer.groups.back()->vertex_length + glyph_vertex_length > 65535)) {
            // Move to a new group because the old one can't hold the geometry.
            buffer.groups.emplace_back(std::make_unique<GroupType>());
        }

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        assert(buffer.groups.back());
        auto &triangleGroup = *buffer.groups.back();
        GLsizei triangleIndex = triangleGroup.vertex_length;

        // Encode angle of glyph
        uint8_t glyphAngle = std::round((symbol.glyphAngle / (M_PI * 2)) * 256);

        // coordinates (2 triangles)
        buffer.vertices.add(anchorPoint.x, anchorPoint.y, tl.x, tl.y, tex.x, tex.y, minZoom,
                            maxZoom, placementZoom, glyphAngle);
        buffer.vertices.add(anchorPoint.x, anchorPoint.y, tr.x, tr.y, tex.x + tex.w, tex.y,
                            minZoom, maxZoom, placementZoom, glyphAngle);
        buffer.vertices.add(anchorPoint.x, anchorPoint.y, bl.x, bl.y, tex.x, tex.y + tex.h,
                            minZoom, maxZoom, placementZoom, glyphAngle);
        buffer.vertices.add(anchorPoint.x, anchorPoint.y, br.x, br.y, tex.x + tex.w, tex.y + tex.h,
                            minZoom, maxZoom, placementZoom, glyphAngle);

        // add the two triangles, referencing the four coordinates we just inserted.
        buffer.triangles.add(triangleIndex + 0, triangleIndex + 1, triangleIndex + 2);
        buffer.triangles.add(triangleIndex + 1, triangleIndex + 2, triangleIndex + 3);

        triangleGroup.vertex_length += glyph_vertex_length;
        triangleGroup.elements_length += 2;
    }
}

void SymbolBucket::addToDebugBuffers(CollisionTile &collisionTile) {

    const float yStretch = collisionTile.yStretch;
    const float angle = collisionTile.config.angle;
    float angle_sin = std::sin(-angle);
    float angle_cos = std::cos(-angle);
    std::array<float, 4> matrix = {{angle_cos, -angle_sin, angle_sin, angle_cos}};

    for (const SymbolInstance &symbolInstance : symbolInstances) {
        for (int i = 0; i < 2; i++) {
            auto& feature = i == 0 ?
                symbolInstance.textCollisionFeature :
                symbolInstance.iconCollisionFeature;

            for (const CollisionBox &box : feature.boxes) {
                auto& anchor = box.anchor;

                Point<float> tl{box.x1, box.y1 * yStretch};
                Point<float> tr{box.x2, box.y1 * yStretch};
                Point<float> bl{box.x1, box.y2 * yStretch};
                Point<float> br{box.x2, box.y2 * yStretch};
                tl = util::matrixMultiply(matrix, tl);
                tr = util::matrixMultiply(matrix, tr);
                bl = util::matrixMultiply(matrix, bl);
                br = util::matrixMultiply(matrix, br);

                const float maxZoom = util::max(0.0f, util::min(25.0f, static_cast<float>(zoom + log(box.maxScale) / log(2))));
                const float placementZoom= util::max(0.0f, util::min(25.0f, static_cast<float>(zoom + log(box.placementScale) / log(2))));

                auto& collisionBox = renderDataInProgress->collisionBox;
                if (collisionBox.groups.empty()) {
                    // Move to a new group because the old one can't hold the geometry.
                    collisionBox.groups.emplace_back(std::make_unique<CollisionBoxElementGroup>());
                }

                collisionBox.vertices.add(anchor.x, anchor.y, tl.x, tl.y, maxZoom, placementZoom);
                collisionBox.vertices.add(anchor.x, anchor.y, tr.x, tr.y, maxZoom, placementZoom);
                collisionBox.vertices.add(anchor.x, anchor.y, tr.x, tr.y, maxZoom, placementZoom);
                collisionBox.vertices.add(anchor.x, anchor.y, br.x, br.y, maxZoom, placementZoom);
                collisionBox.vertices.add(anchor.x, anchor.y, br.x, br.y, maxZoom, placementZoom);
                collisionBox.vertices.add(anchor.x, anchor.y, bl.x, bl.y, maxZoom, placementZoom);
                collisionBox.vertices.add(anchor.x, anchor.y, bl.x, bl.y, maxZoom, placementZoom);
                collisionBox.vertices.add(anchor.x, anchor.y, tl.x, tl.y, maxZoom, placementZoom);

                auto &group= *collisionBox.groups.back();
                group.vertex_length += 8;
            }
        }
    }
}

void SymbolBucket::swapRenderData() {
    if (renderDataInProgress) {
        renderData = std::move(renderDataInProgress);
        uploaded = false;
    }
}

void SymbolBucket::drawGlyphs(SDFShader& shader, gl::ObjectStore& store) {
    GLbyte *vertex_index = BUFFER_OFFSET_0;
    GLbyte *elements_index = BUFFER_OFFSET_0;
    auto& text = renderData->text;
    for (auto &group : text.groups) {
        assert(group);
        group->array[0].bind(shader, text.vertices, text.triangles, vertex_index, store);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * text.vertices.itemSize;
        elements_index += group->elements_length * text.triangles.itemSize;
    }
}

void SymbolBucket::drawIcons(SDFShader& shader, gl::ObjectStore& store) {
    GLbyte *vertex_index = BUFFER_OFFSET_0;
    GLbyte *elements_index = BUFFER_OFFSET_0;
    auto& icon = renderData->icon;
    for (auto &group : icon.groups) {
        assert(group);
        group->array[0].bind(shader, icon.vertices, icon.triangles, vertex_index, store);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * icon.vertices.itemSize;
        elements_index += group->elements_length * icon.triangles.itemSize;
    }
}

void SymbolBucket::drawIcons(IconShader& shader, gl::ObjectStore& store) {
    GLbyte *vertex_index = BUFFER_OFFSET_0;
    GLbyte *elements_index = BUFFER_OFFSET_0;
    auto& icon = renderData->icon;
    for (auto &group : icon.groups) {
        assert(group);
        group->array[1].bind(shader, icon.vertices, icon.triangles, vertex_index, store);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * icon.vertices.itemSize;
        elements_index += group->elements_length * icon.triangles.itemSize;
    }
}

void SymbolBucket::drawCollisionBoxes(CollisionBoxShader& shader, gl::ObjectStore& store) {
    GLbyte *vertex_index = BUFFER_OFFSET_0;
    auto& collisionBox = renderData->collisionBox;
    for (auto &group : collisionBox.groups) {
        group->array[0].bind(shader, collisionBox.vertices, vertex_index, store);
        MBGL_CHECK_ERROR(glDrawArrays(GL_LINES, 0, group->vertex_length));
    }
}
} // namespace mbgl
