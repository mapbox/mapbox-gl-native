#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/style/style_layout.hpp>
#include <mbgl/geometry/text_buffer.hpp>
#include <mbgl/geometry/icon_buffer.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/geometry/anchor.hpp>
#include <mbgl/geometry/resample.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/text/font_stack.hpp>
#include <mbgl/text/placement.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/text/collision.hpp>
#include <mbgl/shader/sdf_shader.hpp>
#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/map/sprite.hpp>

#include <mbgl/util/utf.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/merge_lines.hpp>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
#endif

namespace mbgl {

SymbolBucket::SymbolBucket(Collision &collision_)
    : collision(collision_) {
}

SymbolBucket::~SymbolBucket() {
    // Do not remove. header file only contains forward definitions to unique pointers.
}

void SymbolBucket::upload() {
    if (hasTextData()) {
        text.vertices.upload();
        text.triangles.upload();
    }
    if (hasIconData()) {
        icon.vertices.upload();
        icon.triangles.upload();
    }

    uploaded = true;
}

void SymbolBucket::render(Painter& painter,
                          const StyleLayer& layer_desc,
                          const TileID& id,
                          const mat4& matrix) {
    painter.renderSymbol(*this, layer_desc, id, matrix);
}

bool SymbolBucket::hasData() const { return hasTextData() || hasIconData(); }

bool SymbolBucket::hasTextData() const { return !text.groups.empty(); }

bool SymbolBucket::hasIconData() const { return !icon.groups.empty(); }

bool SymbolBucket::needsDependencies(const GeometryTileLayer& layer,
                                     const FilterExpression& filter,
                                     GlyphStore& glyphStore,
                                     Sprite& sprite) {
    const bool has_text = !layout.text.field.empty() && !layout.text.font.empty();
    const bool has_icon = !layout.icon.image.empty();

    if (!has_text && !has_icon) {
        return false;
    }

    // Determine and load glyph ranges
    std::set<GlyphRange> ranges;

    for (std::size_t i = 0; i < layer.featureCount(); i++) {
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

            if (ft.label.size()) {
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

    if (glyphStore.requestGlyphRangesIfNeeded(layout.text.font, ranges)) {
        return true;
    }

    if (!sprite.isLoaded()) {
        return true;
    }

    return false;
}

void SymbolBucket::addFeatures(uintptr_t tileUID,
                               SpriteAtlas& spriteAtlas,
                               Sprite& sprite,
                               GlyphAtlas& glyphAtlas,
                               GlyphStore& glyphStore) {
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

    float justify = 0.5;
    if (layout.text.justify == TextJustifyType::Right) justify = 1;
    else if (layout.text.justify == TextJustifyType::Left) justify = 0;

    auto fontStack = glyphStore.getFontStack(layout.text.font);

    for (const auto& feature : features) {
        if (!feature.geometry.size()) continue;

        Shaping shaping;
        Rect<uint16_t> image;
        GlyphPositions face;

        // if feature has text, shape the text
        if (feature.label.length()) {
            shaping = fontStack->getShaping(
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
            if (shaping.size()) {
                glyphAtlas.addGlyphs(tileUID, feature.label, layout.text.font, **fontStack, face);
            }
        }

        // if feature has icon, get sprite atlas position
        if (feature.sprite.length()) {
            image = spriteAtlas.getImage(feature.sprite, false);

            if (sprite.getSpritePosition(feature.sprite).sdf) {
                sdfIcons = true;
            }
        }

        // if either shaping or icon position is present, add the feature
        if (shaping.size() || image.hasArea()) {
            for (const auto& line : feature.geometry) {
                if (line.size()) {
                    addFeature(line, shaping, face, image);
                }
            }
        }
    }

    features.clear();
}

bool byScale(const Anchor &a, const Anchor &b) { return a.scale < b.scale; }

const PlacementRange fullRange{{2 * M_PI, 0}};

void SymbolBucket::addFeature(const std::vector<Coordinate> &line, const Shaping &shaping,
                              const GlyphPositions &face, const Rect<uint16_t> &image) {
    assert(line.size());

    const float minScale = 0.5f;
    const float glyphSize = 24.0f;

    const bool horizontalText =
        layout.text.rotation_alignment == RotationAlignmentType::Viewport;
    const bool horizontalIcon =
        layout.icon.rotation_alignment == RotationAlignmentType::Viewport;
    const float fontScale = layout.text.max_size / glyphSize;
    const float textBoxScale = collision.tilePixelRatio * fontScale;
    const float iconBoxScale = collision.tilePixelRatio * layout.icon.max_size;
    const bool iconWithoutText = layout.text.optional || !shaping.size();
    const bool textWithoutIcon = layout.icon.optional || !image.hasArea();
    const bool avoidEdges = layout.avoid_edges && layout.placement != PlacementType::Line;

    Anchors anchors;

    if (layout.placement == PlacementType::Line) {
        float resampleOffset = 0;

        if (shaping.size()) {
            float minX = std::numeric_limits<float>::infinity();
            float maxX = -std::numeric_limits<float>::infinity();
            for (const auto &glyph : shaping) {
                minX = std::min(minX, glyph.x);
                maxX = std::max(maxX, glyph.x);
            }
            const float labelLength = maxX - minX;
            resampleOffset = (labelLength / 2.0 + glyphSize * 2.0) * fontScale;
        }

        // Line labels
        anchors = resample(line, layout.min_distance, minScale, collision.maxPlacementScale,
                           collision.tilePixelRatio, resampleOffset);

        // Sort anchors by segment so that we can start placement with the
        // anchors that can be shown at the lowest zoom levels.
        std::sort(anchors.begin(), anchors.end(), byScale);

    } else {
        // Point labels
        anchors = {Anchor{float(line[0].x), float(line[0].y), 0, minScale}};
    }

    // TODO: figure out correct ascender height.
    const vec2<float> origin = {0, -17};

    for (auto& anchor : anchors) {

        // Calculate the scales at which the text and icons can be first shown without overlap
        Placement glyphPlacement;
        Placement iconPlacement;
        float glyphScale = 0;
        float iconScale = 0;
        const bool inside = !(anchor.x < 0 || anchor.x > 4096 || anchor.y < 0 || anchor.y > 4096);

        if (avoidEdges && !inside) continue;

        if (shaping.size()) {
            glyphPlacement = Placement::getGlyphs(anchor, origin, shaping, face, textBoxScale,
                                                  horizontalText, line, layout);
            glyphScale =
                layout.text.allow_overlap
                    ? glyphPlacement.minScale
                    : collision.getPlacementScale(glyphPlacement.boxes, glyphPlacement.minScale, avoidEdges);
            if (!glyphScale && !iconWithoutText)
                continue;
        }

        if (image.hasArea()) {
            iconPlacement = Placement::getIcon(anchor, image, iconBoxScale, line, layout);
            iconScale =
                layout.icon.allow_overlap
                    ? iconPlacement.minScale
                    : collision.getPlacementScale(iconPlacement.boxes, iconPlacement.minScale, avoidEdges);
            if (!iconScale && !textWithoutIcon)
                continue;
        }

        if (!iconWithoutText && !textWithoutIcon) {
            iconScale = glyphScale = util::max(iconScale, glyphScale);
        } else if (!textWithoutIcon && glyphScale) {
            glyphScale = util::max(iconScale, glyphScale);
        } else if (!iconWithoutText && iconScale) {
            iconScale = util::max(iconScale, glyphScale);
        }

        // Get the rotation ranges it is safe to show the glyphs
        PlacementRange glyphRange =
            (!glyphScale || layout.text.allow_overlap)
                ? fullRange
                : collision.getPlacementRange(glyphPlacement.boxes, glyphScale, horizontalText);
        PlacementRange iconRange =
            (!iconScale || layout.icon.allow_overlap)
                ? fullRange
                : collision.getPlacementRange(iconPlacement.boxes, iconScale, horizontalIcon);

        const PlacementRange maxRange = {{
            util::min(iconRange[0], glyphRange[0]), util::max(iconRange[1], glyphRange[1]),
        }};

        if (!iconWithoutText && !textWithoutIcon) {
            iconRange = glyphRange = maxRange;
        } else if (!textWithoutIcon) {
            glyphRange = maxRange;
        } else if (!iconWithoutText) {
            iconRange = maxRange;
        }

        // Insert final placement into collision tree and add glyphs/icons to buffers
        if (glyphScale && std::isfinite(glyphScale)) {
            if (!layout.text.ignore_placement) {
                collision.insert(glyphPlacement.boxes, anchor, glyphScale, glyphRange,
                                 horizontalText);
            }
            if (inside) addSymbols<TextBuffer, TextElementGroup>(text, glyphPlacement.shapes, glyphScale, glyphRange);
        }

        if (iconScale && std::isfinite(iconScale)) {
            if (!layout.icon.ignore_placement) {
                collision.insert(iconPlacement.boxes, anchor, iconScale, iconRange, horizontalIcon);
            }
            if (inside) addSymbols<IconBuffer, IconElementGroup>(icon, iconPlacement.shapes, iconScale, iconRange);
        }
    }
}

template <typename Buffer, typename GroupType>
void SymbolBucket::addSymbols(Buffer &buffer, const PlacedGlyphs &symbols, float scale,
                              PlacementRange placementRange) {
    const float zoom = collision.zoom;

    const float placementZoom = std::log(scale) / std::log(2) + zoom;

    for (const auto& symbol : symbols) {
        const auto &tl = symbol.tl;
        const auto &tr = symbol.tr;
        const auto &bl = symbol.bl;
        const auto &br = symbol.br;
        const auto &tex = symbol.tex;
        const auto &angle = symbol.angle;

        float minZoom =
            util::max(static_cast<float>(zoom + log(symbol.minScale) / log(2)), placementZoom);
        float maxZoom = util::min(static_cast<float>(zoom + log(symbol.maxScale) / log(2)), 25.0f);
        const auto &glyphAnchor = symbol.anchor;

        if (maxZoom <= minZoom)
            continue;

        // Lower min zoom so that while fading out the label
        // it can be shown outside of collision-free zoom levels
        if (minZoom == placementZoom) {
            minZoom = 0;
        }

        const int glyph_vertex_length = 4;

        if (!buffer.groups.size() ||
            (buffer.groups.back()->vertex_length + glyph_vertex_length > 65535)) {
            // Move to a new group because the old one can't hold the geometry.
            buffer.groups.emplace_back(std::make_unique<GroupType>());
        }

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        assert(buffer.groups.back());
        auto &triangleGroup = *buffer.groups.back();
        uint32_t triangleIndex = triangleGroup.vertex_length;

        // coordinates (2 triangles)
        buffer.vertices.add(glyphAnchor.x, glyphAnchor.y, tl.x, tl.y, tex.x, tex.y, angle, minZoom,
                            placementRange, maxZoom, placementZoom);
        buffer.vertices.add(glyphAnchor.x, glyphAnchor.y, tr.x, tr.y, tex.x + tex.w, tex.y, angle,
                            minZoom, placementRange, maxZoom, placementZoom);
        buffer.vertices.add(glyphAnchor.x, glyphAnchor.y, bl.x, bl.y, tex.x, tex.y + tex.h, angle,
                            minZoom, placementRange, maxZoom, placementZoom);
        buffer.vertices.add(glyphAnchor.x, glyphAnchor.y, br.x, br.y, tex.x + tex.w, tex.y + tex.h,
                            angle, minZoom, placementRange, maxZoom, placementZoom);

        // add the two triangles, referencing the four coordinates we just inserted.
        buffer.triangles.add(triangleIndex + 0, triangleIndex + 1, triangleIndex + 2);
        buffer.triangles.add(triangleIndex + 1, triangleIndex + 2, triangleIndex + 3);

        triangleGroup.vertex_length += glyph_vertex_length;
        triangleGroup.elements_length += 2;
    }
}

void SymbolBucket::drawGlyphs(SDFShader &shader) {
    char *vertex_index = BUFFER_OFFSET(0);
    char *elements_index = BUFFER_OFFSET(0);
    for (auto &group : text.groups) {
        assert(group);
        group->array[0].bind(shader, text.vertices, text.triangles, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * text.vertices.itemSize;
        elements_index += group->elements_length * text.triangles.itemSize;
    }
}

void SymbolBucket::drawIcons(SDFShader &shader) {
    char *vertex_index = BUFFER_OFFSET(0);
    char *elements_index = BUFFER_OFFSET(0);
    for (auto &group : icon.groups) {
        assert(group);
        group->array[0].bind(shader, icon.vertices, icon.triangles, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * icon.vertices.itemSize;
        elements_index += group->elements_length * icon.triangles.itemSize;
    }
}

void SymbolBucket::drawIcons(IconShader &shader) {
    char *vertex_index = BUFFER_OFFSET(0);
    char *elements_index = BUFFER_OFFSET(0);
    for (auto &group : icon.groups) {
        assert(group);
        group->array[1].bind(shader, icon.vertices, icon.triangles, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * icon.vertices.itemSize;
        elements_index += group->elements_length * icon.triangles.itemSize;
    }
}
}
