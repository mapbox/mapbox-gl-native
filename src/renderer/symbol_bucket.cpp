#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/geometry/text_buffer.hpp>
#include <mbgl/geometry/icon_buffer.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/geometry/geometry.hpp>
#include <mbgl/geometry/anchor.hpp>
#include <mbgl/geometry/interpolate.hpp>
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

SymbolBucket::SymbolBucket(const StyleBucketSymbol &properties, Collision &collision)
    : properties(properties), collision(collision) {}

void SymbolBucket::render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc,
                          const Tile::ID &id, const mat4 &matrix) {
    painter.renderSymbol(*this, layer_desc, id, matrix);
}

bool SymbolBucket::hasData() const { return hasTextData() || hasIconData(); }

bool SymbolBucket::hasTextData() const { return !text.groups.empty(); }

bool SymbolBucket::hasIconData() const { return !icon.groups.empty(); }

void SymbolBucket::addGlyphsToAtlas(uint64_t tileid, const std::string stackname,
                                    const std::u32string &string, const FontStack &fontStack,
                                    GlyphAtlas &glyphAtlas, GlyphPositions &face) {
    const std::map<uint32_t, SDFGlyph> &sdfs = fontStack.getSDFs();
    // Loop through all characters and add glyph to atlas, positions.
    for (uint32_t chr : string) {
        auto sdf_it = sdfs.find(chr);
        if (sdf_it != sdfs.end()) {
            const SDFGlyph &sdf = sdf_it->second;
            const Rect<uint16_t> rect = glyphAtlas.addGlyph(tileid, stackname, sdf);
            face.emplace(chr, Glyph{rect, sdf.metrics});
        }
    }
}

std::vector<SymbolFeature> SymbolBucket::processFeatures(const VectorTileLayer &layer,
                                                         const FilterExpression &filter,
                                                         GlyphStore &glyphStore,
                                                         const Sprite &sprite) {
    const bool text = properties.text.field.size();
    const bool icon = properties.icon.image.size();

    std::vector<SymbolFeature> features;

    if (!text && !icon) {
        return features;
    }

    util::utf8_to_utf32 ucs4conv;

    // Determine and load glyph ranges
    std::set<GlyphRange> ranges;

    FilteredVectorTileLayer filtered_layer(layer, filter);
    for (const pbf &feature_pbf : filtered_layer) {
        const VectorTileFeature feature{feature_pbf, layer};

        SymbolFeature ft;

        if (text) {
            std::string u8string = util::replaceTokens(properties.text.field, feature.properties);

            auto &convert = std::use_facet<std::ctype<char>>(std::locale());
            if (properties.text.transform == TextTransformType::Uppercase) {
                convert.toupper(&u8string[0], &u8string[0] + u8string.size());
            } else if (properties.text.transform == TextTransformType::Lowercase) {
                convert.tolower(&u8string[0], &u8string[0] + u8string.size());
            }

            ft.label = ucs4conv.convert(u8string);

            if (ft.label.size()) {
                // Loop through all characters of this text and collect unique codepoints.
                for (char32_t chr : ft.label) {
                    ranges.insert(getGlyphRange(chr));
                }
            }
        }

        if (icon) {
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
                               GlyphAtlas &glyphAtlas, GlyphStore &glyphStore) {

    const std::vector<SymbolFeature> features = processFeatures(layer, filter, glyphStore, sprite);

    float horizontalAlign = 0.5;
    if (properties.text.horizontal_align == TextHorizontalAlignType::Right)
        horizontalAlign = 1;
    else if (properties.text.horizontal_align == TextHorizontalAlignType::Left)
        horizontalAlign = 0;

    float verticalAlign = 0.5;
    if (properties.text.vertical_align == TextVerticalAlignType::Bottom)
        verticalAlign = 1;
    else if (properties.text.vertical_align == TextVerticalAlignType::Top)
        verticalAlign = 0;

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
                addGlyphsToAtlas(id.to_uint64(), properties.text.font, feature.label, fontStack,
                                 glyphAtlas, face);
            }
        }

        // if feature has icon, get sprite atlas position
        if (feature.sprite.length()) {
            image = spriteAtlas.waitForImage(feature.sprite, sprite);
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
        anchors = interpolate(line, properties.min_distance, minScale, collision.maxPlacementScale,
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
        if (glyphScale) {
            if (!properties.text.ignore_placement) {
                collision.insert(glyphPlacement.boxes, anchor, glyphScale, glyphRange,
                                 horizontalText);
            }
            if (inside) addSymbols(text, glyphPlacement.shapes, glyphScale, glyphRange);
        }

        if (iconScale) {
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
        TextElementGroup &triangleGroup = buffer.groups.back();
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

void SymbolBucket::drawGlyphs(TextShader &shader) {
    char *vertex_index = BUFFER_OFFSET(0);
    char *elements_index = BUFFER_OFFSET(0);
    for (TextElementGroup &group : text.groups) {
        group.array[0].bind(shader, text.vertices, text.triangles, vertex_index);
        glDrawElements(GL_TRIANGLES, group.elements_length * 3, GL_UNSIGNED_SHORT, elements_index);
        vertex_index += group.vertex_length * text.vertices.itemSize;
        elements_index += group.elements_length * text.triangles.itemSize;
    }
}

void SymbolBucket::drawIcons(IconShader &shader) {
    char *vertex_index = BUFFER_OFFSET(0);
    char *elements_index = BUFFER_OFFSET(0);
    for (IconElementGroup &group : icon.groups) {
        group.array[0].bind(shader, icon.vertices, icon.triangles, vertex_index);
        glDrawElements(GL_TRIANGLES, group.elements_length * 3, GL_UNSIGNED_SHORT, elements_index);
        vertex_index += group.vertex_length * icon.vertices.itemSize;
        elements_index += group.elements_length * icon.triangles.itemSize;
    }
}
}
