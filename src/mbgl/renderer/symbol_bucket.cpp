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
#include <mbgl/text/quads.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/text/collision.hpp>
#include <mbgl/map/sprite.hpp>

#include <mbgl/util/utf.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/merge_lines.hpp>
#include <mbgl/util/std.hpp>

namespace mbgl {

SymbolInstance::SymbolInstance(Anchor &anchor, const std::vector<Coordinate> &line,
        const Shaping &shapedText, const PositionedIcon &shapedIcon, 
        const StyleLayoutSymbol &layout, const bool inside,
        const float textBoxScale, const float /*textPadding*/, const float textAlongLine,
        const float /*iconBoxScale*/, const float /*iconPadding*/, const float iconAlongLine,
        const GlyphPositions &face) :
    hasText(shapedText), hasIcon(shapedIcon),
    glyphQuads(inside && shapedText ?
            getGlyphQuads(anchor, shapedText, textBoxScale, line, layout, textAlongLine, face) :
            PlacedGlyphs()),
    iconQuads(inside && shapedIcon ?
            getIconQuads(anchor, shapedIcon, line, layout, iconAlongLine) :
            PlacedGlyphs()) {};

SymbolBucket::SymbolBucket(std::unique_ptr<const StyleLayoutSymbol> styleLayout_, Collision &collision_)
    : styleLayout(std::move(styleLayout_)), collision(collision_) {
    assert(styleLayout);
}

SymbolBucket::~SymbolBucket() {
    // Do not remove. header file only contains forward definitions to unique pointers.
}

void SymbolBucket::render(Painter &painter, const StyleLayer &layer_desc, const Tile::ID &id,
                          const mat4 &matrix) {
    painter.renderSymbol(*this, layer_desc, id, matrix);
}

bool SymbolBucket::hasData() const { return hasTextData() || hasIconData(); }

bool SymbolBucket::hasTextData() const { return !text.groups.empty(); }

bool SymbolBucket::hasIconData() const { return !icon.groups.empty(); }

std::vector<SymbolFeature> SymbolBucket::processFeatures(const GeometryTileLayer& layer,
                                                         const FilterExpression& filter,
                                                         GlyphStore &glyphStore,
                                                         const Sprite &sprite) {
    auto &layout = *styleLayout;
    const bool has_text = layout.text.field.size();
    const bool has_icon = layout.icon.image.size();

    std::vector<SymbolFeature> features;

    if (!has_text && !has_icon) {
        return features;
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

    glyphStore.waitForGlyphRanges(layout.text.font, ranges);
    sprite.waitUntilLoaded();

    return features;
}

void SymbolBucket::addFeatures(const GeometryTileLayer& layer,
                               const FilterExpression& filter,
                               uintptr_t tileUID,
                               SpriteAtlas& spriteAtlas,
                               Sprite& sprite,
                               GlyphAtlas& glyphAtlas,
                               GlyphStore& glyphStore) {
    auto &layout = *styleLayout;
    const std::vector<SymbolFeature> features = processFeatures(layer, filter, glyphStore, sprite);

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

    const auto &fontStack = glyphStore.getFontStack(layout.text.font);

    for (const SymbolFeature &feature : features) {
        if (!feature.geometry.size()) continue;

        Shaping shapedText;
        PositionedIcon shapedIcon;
        GlyphPositions face;

        // if feature has text, shape the text
        if (feature.label.length()) {
            shapedText = fontStack->getShaping(
                /* string */ feature.label,
                /* maxWidth: ems */ layout.text.max_width * 24,
                /* lineHeight: ems */ layout.text.line_height * 24,
                /* horizontalAlign */ horizontalAlign,
                /* verticalAlign */ verticalAlign,
                /* justify */ justify,
                /* spacing: ems */ layout.text.letter_spacing * 24,
                /* translate */ vec2<float>(layout.text.offset[0], layout.text.offset[1]));

            // Add the glyphs we need for this label to the glyph atlas.
            if (shapedText) {
                glyphAtlas.addGlyphs(tileUID, feature.label, layout.text.font, fontStack, face);
            }
        }

        // if feature has icon, get sprite atlas position
        if (feature.sprite.length()) {
            sprite.waitUntilLoaded();
            Rect<uint16_t> image = spriteAtlas.getImage(feature.sprite, false);
            shapedIcon = shapeIcon(image, layout);

            if (sprite.getSpritePosition(feature.sprite).sdf) {
                sdfIcons = true;
            }
        }

        // if either shapedText or icon position is present, add the feature
        if (shapedText || shapedIcon) {
            addFeature(feature.geometry, shapedText, shapedIcon, face);
        }
    }

    placeFeatures();
}


void SymbolBucket::addFeature(const std::vector<std::vector<Coordinate>> &lines,
                    const Shaping &shapedText, const PositionedIcon &shapedIcon, const GlyphPositions &face) {
    auto &layout = *styleLayout;

    const float minScale = 0.5f;
    const float glyphSize = 24.0f;

    const float fontScale = layout.text.max_size / glyphSize;
    const float textBoxScale = collision.tilePixelRatio * fontScale;
    const float iconBoxScale = collision.tilePixelRatio * layout.icon.max_size;
    //const float symbolSpacing = collision.tilePixelRatio * layout.min_distance;
    const bool avoidEdges = layout.avoid_edges && layout.placement != PlacementType::Line;
    const float textPadding = layout.text.padding * collision.tilePixelRatio;
    const float iconPadding = layout.icon.padding * collision.tilePixelRatio;
    //const float textMaxAngle = layout.text.max_angle * M_PI / 180;
    const bool textAlongLine =
        layout.text.rotation_alignment != RotationAlignmentType::Viewport &&
        layout.placement == PlacementType::Line;
    const bool iconAlongLine =
        layout.icon.rotation_alignment != RotationAlignmentType::Viewport &&
        layout.placement == PlacementType::Line;

    // TODO clip lines here

    for (const std::vector<Coordinate> &line : lines) {
        if (!line.size()) continue;

        // Calculate the anchor points around which you want to place labels
        Anchors anchors = layout.placement == PlacementType::Line ?
            resample(line, layout.min_distance, minScale, collision.maxPlacementScale, collision.tilePixelRatio, 0.0f) :
            Anchors({ Anchor(float(line[0].x), float(line[0].y), 0, minScale) });


        // For each potential label, create the placement features used to check for collisions, and the quads use for rendering.
        for (Anchor &anchor : anchors) {

            const bool inside = !(anchor.x < 0 || anchor.x > 4096 || anchor.y < 0 || anchor.y > 4096);

            if (avoidEdges && !inside) continue;

            symbolInstances.emplace_back(anchor, line, shapedText, shapedIcon, layout, inside,
                    textBoxScale, textPadding, textAlongLine,
                    iconBoxScale, iconPadding, iconAlongLine,
                    face);
        }
    }
}

void SymbolBucket::placeFeatures() {

    //auto &layout = *styleLayout;

    for (SymbolInstance &symbolInstance : symbolInstances) {

        const bool hasText = symbolInstance.hasText;
        const bool hasIcon = symbolInstance.hasIcon;

        float glyphScale = 0.5f;
        float iconScale = 0.5f;

        /*
           if (!iconWithoutText && !textWithoutIcon) {
           iconScale = glyphScale = util::max(iconScale, glyphScale);
           } else if (!textWithoutIcon && glyphScale) {
           glyphScale = util::max(iconScale, glyphScale);
           } else if (!iconWithoutText && iconScale) {
           iconScale = util::max(iconScale, glyphScale);
           }
        */

        // Insert final placement into collision tree and add glyphs/icons to buffers
        if (hasText && std::isfinite(glyphScale)) {
            addSymbols<TextBuffer, TextElementGroup>(text, symbolInstance.glyphQuads, glyphScale);
        }

        if (hasIcon && std::isfinite(iconScale)) {
            addSymbols<IconBuffer, IconElementGroup>(icon, symbolInstance.iconQuads, iconScale);
        }
    }
}

template <typename Buffer, typename GroupType>
void SymbolBucket::addSymbols(Buffer &buffer, const PlacedGlyphs &symbols, float scale) {
    const float zoom = collision.zoom;

    const float placementZoom = std::log(scale) / std::log(2) + zoom;

    for (const PlacedGlyph &symbol : symbols) {
        const auto &tl = symbol.tl;
        const auto &tr = symbol.tr;
        const auto &bl = symbol.bl;
        const auto &br = symbol.br;
        const auto &tex = symbol.tex;

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
            buffer.groups.emplace_back(util::make_unique<GroupType>());
        }

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        assert(buffer.groups.back());
        auto &triangleGroup = *buffer.groups.back();
        uint32_t triangleIndex = triangleGroup.vertex_length;

        // coordinates (2 triangles)
        buffer.vertices.add(glyphAnchor.x, glyphAnchor.y, tl.x, tl.y, tex.x, tex.y, minZoom,
                            maxZoom, placementZoom);
        buffer.vertices.add(glyphAnchor.x, glyphAnchor.y, tr.x, tr.y, tex.x + tex.w, tex.y,
                            minZoom, maxZoom, placementZoom);
        buffer.vertices.add(glyphAnchor.x, glyphAnchor.y, bl.x, bl.y, tex.x, tex.y + tex.h,
                            minZoom, maxZoom, placementZoom);
        buffer.vertices.add(glyphAnchor.x, glyphAnchor.y, br.x, br.y, tex.x + tex.w, tex.y + tex.h,
                            minZoom, maxZoom, placementZoom);

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
