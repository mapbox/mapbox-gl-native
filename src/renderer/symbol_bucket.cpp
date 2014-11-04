#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/geometry/text_buffer.hpp>
#include <mbgl/geometry/icon_buffer.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/geometry/geometry.hpp>
#include <mbgl/geometry/anchor.hpp>
#include <mbgl/geometry/resample.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/text/placement.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/text/collision.hpp>
#include <mbgl/map/sprite.hpp>

#include <mbgl/util/utf.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

SymbolBucket::SymbolBucket(const StyleBucketSymbol &properties_, Collision &collision_)
    : properties(properties_), collision(collision_) {}

void SymbolBucket::render(Painter &painter, util::ptr<StyleLayer> layer_desc,
                          const Tile::ID &id, const mat4 &matrix) {
    painter.renderSymbol(*this, layer_desc, id, matrix);
}

bool SymbolBucket::hasData() const { return hasTextData() || hasIconData(); }

bool SymbolBucket::hasTextData() const { return !text.groups.empty(); }

bool SymbolBucket::hasIconData() const { return !icon.groups.empty(); }

void SymbolBucket::addGlyphsToAtlas(uint64_t tileid, const std::string stackname,
                                    const std::u32string &text, const FontStack &fontStack,
                                    GlyphAtlas &glyphAtlas, GlyphPositions &face) {
    glyphAtlas.addGlyphs(tileid, text, stackname, fontStack,face);
}

std::vector<SymbolFeature> SymbolBucket::processFeatures(const VectorTileLayer &layer,
                                                         const FilterExpression &filter,
                                                         GlyphStore &glyphStore,
                                                         const Sprite &sprite) {
    const bool has_text = properties.text.field.size();
    const bool has_icon = properties.icon.image.size();

    std::vector<SymbolFeature> features;

    if (!has_text && !has_icon) {
        return features;
    }

    // Determine and load glyph ranges
    std::set<GlyphRange> ranges;

    FilteredVectorTileLayer filtered_layer(layer, filter);
    for (const pbf &feature_pbf : filtered_layer) {
        const VectorTileFeature feature{feature_pbf, layer};

        SymbolFeature ft;

        if (has_text) {
            std::string u8string = util::replaceTokens(properties.text.field, feature.properties);

            if (properties.text.transform == TextTransformType::Uppercase) {
                u8string = platform::uppercase(u8string);
            } else if (properties.text.transform == TextTransformType::Lowercase) {
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
            ft.sprite = util::replaceTokens(properties.icon.image, feature.properties);
        }

        if (ft.label.length() || ft.sprite.length()) {
            ft.geometry = feature.geometry;
            features.push_back(std::move(ft));
        }
    }

    glyphStore.waitForGlyphRanges(properties.text.font, ranges);
    sprite.waitUntilLoaded();

    return features;
}

void SymbolBucket::addFeatures(const VectorTileLayer &layer, const FilterExpression &filter,
                               const Tile::ID &id, SpriteAtlas &spriteAtlas, Sprite &sprite,
                               GlyphAtlas & glyphAtlas, GlyphStore &glyphStore) {

    const std::vector<SymbolFeature> features = processFeatures(layer, filter, glyphStore, sprite);

    float horizontalAlign = 0.5;
    float verticalAlign = 0.5;

    switch (properties.text.anchor) {
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

    switch (properties.text.anchor) {
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
    if (properties.text.justify == TextJustifyType::Right) justify = 1;
    else if (properties.text.justify == TextJustifyType::Left) justify = 0;

    const FontStack &fontStack = glyphStore.getFontStack(properties.text.font);

    for (const SymbolFeature &feature : features) {
        Shaping shaping;
        Rect<uint16_t> image;
        GlyphPositions face;

        // if feature has text, shape the text
        if (feature.label.length()) {
            shaping = fontStack.getShaping(
                /* string */ feature.label,
                /* maxWidth */ properties.text.max_width,
                /* lineHeight */ properties.text.line_height,
                /* horizontalAlign */ horizontalAlign,
                /* verticalAlign */ verticalAlign,
                /* justify */ justify,
                /* spacing */ properties.text.letter_spacing,
                /* translate */ properties.text.offset);

            // Add the glyphs we need for this label to the glyph atlas.
            if (shaping.size()) {
                SymbolBucket::addGlyphsToAtlas(id.to_uint64(), properties.text.font, feature.label, fontStack,
                                               glyphAtlas, face);
            }
        }

        // if feature has icon, get sprite atlas position
        if (feature.sprite.length()) {
            sprite.waitUntilLoaded();
            image = spriteAtlas.getImage(feature.sprite);

            if (sprite.getSpritePosition(feature.sprite).sdf) {
                sdfIcons = true;
            }
        }

        // if either shaping or icon position is present, add the feature
        if (shaping.size() || image) {
            addFeature(feature.geometry, shaping, face, image);
        }
    }
}

void SymbolBucket::addFeature(const pbf &geom_pbf, const Shaping &shaping,
                              const GlyphPositions &face, const Rect<uint16_t> &image) {
    // Decode all lines.
    std::vector<Coordinate> line;
    Geometry::command cmd;

    Coordinate coord;
    pbf geom(geom_pbf);
    Geometry geometry(geom);
    int32_t x, y;
    while ((cmd = geometry.next(x, y)) != Geometry::end) {
        if (cmd == Geometry::move_to) {
            if (!line.empty()) {
                addFeature(line, shaping, face, image);
                line.clear();
            }
        }
        line.emplace_back(x, y);
    }
    if (line.size()) {
        addFeature(line, shaping, face, image);
    }
}

bool byScale(const Anchor &a, const Anchor &b) { return a.scale < b.scale; }

const PlacementRange fullRange{{2 * M_PI, 0}};

void SymbolBucket::addFeature(const std::vector<Coordinate> &line, const Shaping &shaping,
                              const GlyphPositions &face, const Rect<uint16_t> &image) {
    assert(line.size());

    const float minScale = 0.5f;
    const float glyphSize = 24.0f;

    const bool horizontalText =
        properties.text.rotation_alignment == RotationAlignmentType::Viewport;
    const bool horizontalIcon =
        properties.icon.rotation_alignment == RotationAlignmentType::Viewport;
    const float fontScale = properties.text.max_size / glyphSize;
    const float textBoxScale = collision.tilePixelRatio * fontScale;
    const float iconBoxScale = collision.tilePixelRatio * properties.icon.max_size;
    const bool iconWithoutText = properties.text.optional || !shaping.size();
    const bool textWithoutIcon = properties.icon.optional || !image;
    const bool avoidEdges = properties.avoid_edges && properties.placement != PlacementType::Line;

    Anchors anchors;

    if (properties.placement == PlacementType::Line) {
        // Line labels
        anchors = resample(line, properties.min_distance, minScale, collision.maxPlacementScale,
                           collision.tilePixelRatio);

        // Sort anchors by segment so that we can start placement with the
        // anchors that can be shown at the lowest zoom levels.
        std::sort(anchors.begin(), anchors.end(), byScale);

    } else {
        // Point labels
        anchors = {Anchor{float(line[0].x), float(line[0].y), 0, minScale}};
    }

    // TODO: figure out correct ascender height.
    const vec2<float> origin = {0, -17};

    for (Anchor &anchor : anchors) {

        // Calculate the scales at which the text and icons can be first shown without overlap
        Placement glyphPlacement;
        Placement iconPlacement;
        float glyphScale = 0;
        float iconScale = 0;
        const bool inside = !(anchor.x < 0 || anchor.x > 4096 || anchor.y < 0 || anchor.y > 4096);

        if (avoidEdges && !inside) continue;

        if (shaping.size()) {
            glyphPlacement = Placement::getGlyphs(anchor, origin, shaping, face, textBoxScale,
                                                  horizontalText, line, properties);
            glyphScale =
                properties.text.allow_overlap
                    ? glyphPlacement.minScale
                    : collision.getPlacementScale(glyphPlacement.boxes, glyphPlacement.minScale, avoidEdges);
            if (!glyphScale && !iconWithoutText)
                continue;
        }

        if (image) {
            iconPlacement = Placement::getIcon(anchor, image, iconBoxScale, line, properties);
            iconScale =
                properties.icon.allow_overlap
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
            (!glyphScale || properties.text.allow_overlap)
                ? fullRange
                : collision.getPlacementRange(glyphPlacement.boxes, glyphScale, horizontalText);
        PlacementRange iconRange =
            (!iconScale || properties.icon.allow_overlap)
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
            if (!properties.text.ignore_placement) {
                collision.insert(glyphPlacement.boxes, anchor, glyphScale, glyphRange,
                                 horizontalText);
            }
            if (inside) addSymbols(text, glyphPlacement.shapes, glyphScale, glyphRange);
        }

        if (iconScale && std::isfinite(iconScale)) {
            if (!properties.icon.ignore_placement) {
                collision.insert(iconPlacement.boxes, anchor, iconScale, iconRange, horizontalIcon);
            }
            if (inside) addSymbols(icon, iconPlacement.shapes, iconScale, iconRange);
        }
    }
}

template <typename Buffer>
void SymbolBucket::addSymbols(Buffer &buffer, const PlacedGlyphs &symbols, float scale,
                              PlacementRange placementRange) {
    const float zoom = collision.zoom;

    const float placementZoom = std::log(scale) / std::log(2) + zoom;

    for (const PlacedGlyph &symbol : symbols) {
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
            (buffer.groups.back().vertex_length + glyph_vertex_length > 65535)) {
            // Move to a new group because the old one can't hold the geometry.
            buffer.groups.emplace_back();
        }

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        auto &triangleGroup = buffer.groups.back();
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
    for (TextElementGroup &group : text.groups) {
        group.array[0].bind(shader, text.vertices, text.triangles, vertex_index);
        glDrawElements(GL_TRIANGLES, group.elements_length * 3, GL_UNSIGNED_SHORT, elements_index);
        vertex_index += group.vertex_length * text.vertices.itemSize;
        elements_index += group.elements_length * text.triangles.itemSize;
    }
}

void SymbolBucket::drawIcons(SDFShader &shader) {
    char *vertex_index = BUFFER_OFFSET(0);
    char *elements_index = BUFFER_OFFSET(0);
    for (IconElementGroup &group : icon.groups) {
        group.array[0].bind(shader, icon.vertices, icon.triangles, vertex_index);
        glDrawElements(GL_TRIANGLES, group.elements_length * 3, GL_UNSIGNED_SHORT, elements_index);
        vertex_index += group.vertex_length * icon.vertices.itemSize;
        elements_index += group.elements_length * icon.triangles.itemSize;
    }
}

void SymbolBucket::drawIcons(IconShader &shader) {
    char *vertex_index = BUFFER_OFFSET(0);
    char *elements_index = BUFFER_OFFSET(0);
    for (IconElementGroup &group : icon.groups) {
        group.array[1].bind(shader, icon.vertices, icon.triangles, vertex_index);
        glDrawElements(GL_TRIANGLES, group.elements_length * 3, GL_UNSIGNED_SHORT, elements_index);
        vertex_index += group.vertex_length * icon.vertices.itemSize;
        elements_index += group.elements_length * icon.triangles.itemSize;
    }
}
}
