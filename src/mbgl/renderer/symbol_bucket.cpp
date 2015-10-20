#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/style/style_properties.hpp>
#include <mbgl/annotation/sprite_image.hpp>
#include <mbgl/geometry/text_buffer.hpp>
#include <mbgl/geometry/icon_buffer.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/geometry/anchor.hpp>
#include <mbgl/text/get_anchors.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/text/font_stack.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/shader/sdf_shader.hpp>
#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/shader/box_shader.hpp>
#include <mbgl/map/sprite.hpp>

#include <mbgl/util/utf.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/merge_lines.hpp>
#include <mbgl/util/clip_lines.hpp>
#include <mbgl/util/std.hpp>

namespace mbgl {

SymbolInstance::SymbolInstance(Anchor& anchor, const std::vector<Coordinate>& line,
        const Shaping& shapedText, const PositionedIcon& shapedIcon,
        const SymbolLayoutProperties& layout, const bool addToBuffers,
        const float textBoxScale, const float textPadding, const float textAlongLine,
        const float iconBoxScale, const float iconPadding, const float iconAlongLine,
        const GlyphPositions& face) :
    x(anchor.x),
    y(anchor.y),
    hasText(shapedText),
    hasIcon(shapedIcon),

    // Create the quads used for rendering the glyphs.
    glyphQuads(addToBuffers && shapedText ?
            getGlyphQuads(anchor, shapedText, textBoxScale, line, layout, textAlongLine, face) :
            SymbolQuads()),

    // Create the quad used for rendering the icon.
    iconQuads(addToBuffers && shapedIcon ?
            getIconQuads(anchor, shapedIcon, line, layout, iconAlongLine) :
            SymbolQuads()),

    // Create the collision features that will be used to check whether this symbol instance can be placed
    textCollisionFeature(line, anchor, shapedText, textBoxScale, textPadding, textAlongLine),
    iconCollisionFeature(line, anchor, shapedIcon, iconBoxScale, iconPadding, iconAlongLine) {};


SymbolBucket::SymbolBucket(float overscaling_, float zoom_)
    : overscaling(overscaling_), zoom(zoom_), tileSize(512 * overscaling_), tilePixelRatio(tileExtent / tileSize) {
}

SymbolBucket::~SymbolBucket() {
    // Do not remove. header file only contains forward definitions to unique pointers.
}

void SymbolBucket::upload() {
    if (hasTextData()) {
        renderData->text.vertices.upload();
        renderData->text.triangles.upload();
    }
    if (hasIconData()) {
        renderData->icon.vertices.upload();
        renderData->icon.triangles.upload();
    }

    uploaded = true;
}

void SymbolBucket::render(Painter& painter,
                          const StyleLayer& layer,
                          const TileID& id,
                          const mat4& matrix) {
    painter.renderSymbol(*this, dynamic_cast<const SymbolLayer&>(layer), id, matrix);
}

bool SymbolBucket::hasData() const { return hasTextData() || hasIconData() || !symbolInstances.empty(); }

bool SymbolBucket::hasTextData() const { return renderData && !renderData->text.groups.empty(); }

bool SymbolBucket::hasIconData() const { return renderData && !renderData->icon.groups.empty(); }

bool SymbolBucket::hasCollisionBoxData() const { return renderData && !renderData->collisionBox.groups.empty(); }

void SymbolBucket::parseFeatures(const GeometryTileLayer& layer,
                                 const FilterExpression& filter) {
    const bool has_text = !layout.text.field.empty() && !layout.text.font.empty();
    const bool has_icon = !layout.icon.image.empty();

    if (!has_text && !has_icon) {
        return;
    }

    // Determine and load glyph ranges
    const GLsizei featureCount = static_cast<GLsizei>(layer.featureCount());
    for (GLsizei i = 0; i < featureCount; i++) {
        auto feature = layer.getFeature(i);

        GeometryTileFeatureExtractor extractor(*feature);
        if (!evaluate(filter, extractor))
            continue;

        SymbolFeature ft;

        auto getValue = [&feature](const std::string& key) -> std::string {
            auto value = feature->getValue(key);
            return value ? toString(*value) : std::string();
        };

        if (has_text) {
            std::string u8string = util::replaceTokens(layout.text.field, getValue);

            if (layout.text.transform == TextTransformType::Uppercase) {
                u8string = platform::uppercase(u8string);
            } else if (layout.text.transform == TextTransformType::Lowercase) {
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
            ft.sprite = util::replaceTokens(layout.icon.image, getValue);
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

    if (layout.placement == PlacementType::Line) {
        util::mergeLines(features);
    }
}

bool SymbolBucket::needsDependencies(GlyphStore& glyphStore,
                                     Sprite& sprite) {
    if (!layout.text.field.empty() && !layout.text.font.empty() && !glyphStore.hasGlyphRanges(layout.text.font, ranges)) {
        return true;
    }

    if (!layout.icon.image.empty() && !sprite.isLoaded()) {
        return true;
    }

    return false;
}

void SymbolBucket::addFeatures(uintptr_t tileUID,
                               SpriteAtlas& spriteAtlas,
                               GlyphAtlas& glyphAtlas,
                               GlyphStore& glyphStore,
                               CollisionTile& collisionTile) {
    float horizontalAlign = 0.5;
    float verticalAlign = 0.5;

    switch (layout.text.anchor) {
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

    switch (layout.text.anchor) {
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

    const float justify = layout.text.justify == TextJustifyType::Right ? 1 :
        layout.text.justify == TextJustifyType::Left ? 0 :
        0.5;

    auto fontStack = glyphStore.getFontStack(layout.text.font);

    for (const auto& feature : features) {
        if (feature.geometry.empty()) continue;

        Shaping shapedText;
        PositionedIcon shapedIcon;
        GlyphPositions face;

        // if feature has text, shape the text
        if (feature.label.length()) {
            shapedText = fontStack->getShaping(
                /* string */ feature.label,
                /* maxWidth: ems */ layout.placement != PlacementType::Line ?
                    layout.text.max_width * 24 : 0,
                /* lineHeight: ems */ layout.text.line_height * 24,
                /* horizontalAlign */ horizontalAlign,
                /* verticalAlign */ verticalAlign,
                /* justify */ justify,
                /* spacing: ems */ layout.text.letter_spacing * 24,
                /* translate */ vec2<float>(layout.text.offset[0], layout.text.offset[1]));

            // Add the glyphs we need for this label to the glyph atlas.
            if (shapedText) {
                glyphAtlas.addGlyphs(tileUID, feature.label, layout.text.font, **fontStack, face);
            }
        }

        // if feature has icon, get sprite atlas position
        if (feature.sprite.length()) {
            auto image = spriteAtlas.getImage(feature.sprite, false);
            if (image.pos.hasArea() && image.texture) {
                shapedIcon = shapeIcon(image.pos, layout);
                assert(image.texture);
                if (image.texture->sdf) {
                    sdfIcons = true;
                }
            }
        }

        // if either shapedText or icon position is present, add the feature
        if (shapedText || shapedIcon) {
            addFeature(feature.geometry, shapedText, shapedIcon, face);
        }
    }

    features.clear();

    placeFeatures(collisionTile, true);
}


void SymbolBucket::addFeature(const std::vector<std::vector<Coordinate>> &lines,
        const Shaping &shapedText, const PositionedIcon &shapedIcon, const GlyphPositions &face) {

    const float minScale = 0.5f;
    const float glyphSize = 24.0f;

    const float fontScale = layout.text.size / glyphSize;
    const float textBoxScale = tilePixelRatio * fontScale;
    const float textMaxBoxScale = tilePixelRatio * layout.text.max_size / glyphSize;
    const float iconBoxScale = tilePixelRatio * layout.icon.size;
    const float symbolSpacing = tilePixelRatio * layout.spacing;
    const bool avoidEdges = layout.avoid_edges && layout.placement != PlacementType::Line;
    const float textPadding = layout.text.padding * tilePixelRatio;
    const float iconPadding = layout.icon.padding * tilePixelRatio;
    const float textMaxAngle = layout.text.max_angle * M_PI / 180;
    const bool textAlongLine =
        layout.text.rotation_alignment == RotationAlignmentType::Map &&
        layout.placement == PlacementType::Line;
    const bool iconAlongLine =
        layout.icon.rotation_alignment == RotationAlignmentType::Map &&
        layout.placement == PlacementType::Line;
    const bool mayOverlap = layout.text.allow_overlap || layout.icon.allow_overlap ||
        layout.text.ignore_placement || layout.icon.ignore_placement;
    const bool isLine = layout.placement == PlacementType::Line;
    const float textRepeatDistance = symbolSpacing / 2;

    auto& clippedLines = isLine ?
        util::clipLines(lines, 0, 0, 4096, 4096) :
        lines;

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

            const bool inside = !(anchor.x < 0 || anchor.x > 4096 || anchor.y < 0 || anchor.y > 4096);

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
            const bool addToBuffers = inside || (mayOverlap && false);

            symbolInstances.emplace_back(anchor, line, shapedText, shapedIcon, layout, addToBuffers,
                    textBoxScale, textPadding, textAlongLine,
                    iconBoxScale, iconPadding, iconAlongLine,
                    face);
        }
    }
}
    
bool SymbolBucket::anchorIsTooClose(const std::u32string &text, const float repeatDistance, Anchor &anchor) {
    if (compareText.find(text) == compareText.end()) {
        compareText.emplace(text, Anchors());
    } else {
        auto otherAnchors = compareText.find(text)->second;
        for (Anchor &otherAnchor : otherAnchors) {
            if (util::dist<float>(anchor, otherAnchor) < repeatDistance) {
                return true;
            }
        }
    }
    compareText[text].push_back(anchor);
    return false;
}

void SymbolBucket::placeFeatures(CollisionTile& collisionTile) {
    placeFeatures(collisionTile, false);
}

void SymbolBucket::placeFeatures(CollisionTile& collisionTile, bool swapImmediately) {

    renderDataInProgress = std::make_unique<SymbolRenderData>();

    // Calculate which labels can be shown and when they can be shown and
    // create the bufers used for rendering.

    const bool textAlongLine =
        layout.text.rotation_alignment == RotationAlignmentType::Map &&
        layout.placement == PlacementType::Line;
    const bool iconAlongLine =
        layout.icon.rotation_alignment == RotationAlignmentType::Map &&
        layout.placement == PlacementType::Line;

    const bool mayOverlap = layout.text.allow_overlap || layout.icon.allow_overlap ||
        layout.text.ignore_placement || layout.icon.ignore_placement;

    // Sort symbols by their y position on the canvas so that they lower symbols
    // are drawn on top of higher symbols.
    // Don't sort symbols that won't overlap because it isn't necessary and
    // because it causes more labels to pop in and out when rotating.
    if (mayOverlap) {
        float sin = std::sin(collisionTile.angle);
        float cos = std::cos(collisionTile.angle);

        std::sort(symbolInstances.begin(), symbolInstances.end(), [sin, cos](SymbolInstance &a, SymbolInstance &b) {
            const float aRotated = sin * a.x + cos * a.y;
            const float bRotated = sin * b.x + cos * b.y;
            return aRotated < bRotated;
        });
    }

    for (SymbolInstance &symbolInstance : symbolInstances) {

        const bool hasText = symbolInstance.hasText;
        const bool hasIcon = symbolInstance.hasIcon;

        const bool iconWithoutText = layout.text.optional || !hasText;
        const bool textWithoutIcon = layout.icon.optional || !hasIcon;

        // Calculate the scales at which the text and icon can be placed without collision.

        float glyphScale = hasText && !layout.text.allow_overlap ?
            collisionTile.placeFeature(symbolInstance.textCollisionFeature) : collisionTile.minScale;
        float iconScale = hasIcon && !layout.icon.allow_overlap ?
            collisionTile.placeFeature(symbolInstance.iconCollisionFeature) : collisionTile.minScale;


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
            if (!layout.text.ignore_placement) {
                collisionTile.insertFeature(symbolInstance.textCollisionFeature, glyphScale);
            }
            if (glyphScale < collisionTile.maxScale) {
                addSymbols<SymbolRenderData::TextBuffer, TextElementGroup>(renderDataInProgress->text,
                        symbolInstance.glyphQuads, glyphScale, layout.text.keep_upright, textAlongLine, collisionTile.angle);
            }
        }

        if (hasIcon) {
            if (!layout.icon.ignore_placement) {
                collisionTile.insertFeature(symbolInstance.iconCollisionFeature, iconScale);
            }
            if (iconScale < collisionTile.maxScale) {
                addSymbols<SymbolRenderData::IconBuffer, IconElementGroup>(renderDataInProgress->icon,
                        symbolInstance.iconQuads, iconScale, layout.icon.keep_upright, iconAlongLine, collisionTile.angle);
            }
        }
    }

    if (collisionTile.getDebug()) addToDebugBuffers(collisionTile);

    if (swapImmediately) swapRenderData();
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
        const float a = std::fmod(symbol.angle + placementAngle + M_PI, M_PI * 2);
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

        // coordinates (2 triangles)
        buffer.vertices.add(anchorPoint.x, anchorPoint.y, tl.x, tl.y, tex.x, tex.y, minZoom,
                            maxZoom, placementZoom);
        buffer.vertices.add(anchorPoint.x, anchorPoint.y, tr.x, tr.y, tex.x + tex.w, tex.y,
                            minZoom, maxZoom, placementZoom);
        buffer.vertices.add(anchorPoint.x, anchorPoint.y, bl.x, bl.y, tex.x, tex.y + tex.h,
                            minZoom, maxZoom, placementZoom);
        buffer.vertices.add(anchorPoint.x, anchorPoint.y, br.x, br.y, tex.x + tex.w, tex.y + tex.h,
                            minZoom, maxZoom, placementZoom);

        // add the two triangles, referencing the four coordinates we just inserted.
        buffer.triangles.add(triangleIndex + 0, triangleIndex + 1, triangleIndex + 2);
        buffer.triangles.add(triangleIndex + 1, triangleIndex + 2, triangleIndex + 3);

        triangleGroup.vertex_length += glyph_vertex_length;
        triangleGroup.elements_length += 2;
    }
}

void SymbolBucket::addToDebugBuffers(CollisionTile &collisionTile) {

    const float yStretch = collisionTile.yStretch;
    const float angle = collisionTile.angle;
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

                vec2<float> tl{box.x1, box.y1 * yStretch};
                vec2<float> tr{box.x2, box.y1 * yStretch};
                vec2<float> bl{box.x1, box.y2 * yStretch};
                vec2<float> br{box.x2, box.y2 * yStretch};
                tl = tl.matMul(matrix);
                tr = tr.matMul(matrix);
                bl = bl.matMul(matrix);
                br = br.matMul(matrix);

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
    renderData = std::move(renderDataInProgress);
}

void SymbolBucket::drawGlyphs(SDFShader &shader) {
    GLbyte *vertex_index = BUFFER_OFFSET_0;
    GLbyte *elements_index = BUFFER_OFFSET_0;
    auto& text = renderData->text;
    for (auto &group : text.groups) {
        assert(group);
        group->array[0].bind(shader, text.vertices, text.triangles, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * text.vertices.itemSize;
        elements_index += group->elements_length * text.triangles.itemSize;
    }
}

void SymbolBucket::drawIcons(SDFShader &shader) {
    GLbyte *vertex_index = BUFFER_OFFSET_0;
    GLbyte *elements_index = BUFFER_OFFSET_0;
    auto& icon = renderData->icon;
    for (auto &group : icon.groups) {
        assert(group);
        group->array[0].bind(shader, icon.vertices, icon.triangles, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * icon.vertices.itemSize;
        elements_index += group->elements_length * icon.triangles.itemSize;
    }
}

void SymbolBucket::drawIcons(IconShader &shader) {
    GLbyte *vertex_index = BUFFER_OFFSET_0;
    GLbyte *elements_index = BUFFER_OFFSET_0;
    auto& icon = renderData->icon;
    for (auto &group : icon.groups) {
        assert(group);
        group->array[1].bind(shader, icon.vertices, icon.triangles, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * icon.vertices.itemSize;
        elements_index += group->elements_length * icon.triangles.itemSize;
    }
}

void SymbolBucket::drawCollisionBoxes(CollisionBoxShader &shader) {
    GLbyte *vertex_index = BUFFER_OFFSET_0;
    auto& collisionBox = renderData->collisionBox;
    for (auto &group : collisionBox.groups) {
        group->array[0].bind(shader, collisionBox.vertices, vertex_index);
        MBGL_CHECK_ERROR(glDrawArrays(GL_LINES, 0, group->vertex_length));
    }
}
}
