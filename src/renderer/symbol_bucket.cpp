#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/geometry/text_buffer.hpp>
#include <mbgl/geometry/icon_buffer.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/geometry/geometry.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/text/placement.hpp>
#include <mbgl/platform/log.hpp>

#include <mbgl/util/utf.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/math.hpp>


namespace mbgl {

SymbolBucket::SymbolBucket(TextVertexBuffer &textVertexBuffer, IconVertexBuffer &iconVertexBuffer,
                           TriangleElementsBuffer &triangleElementsBuffer,
                           const StyleBucketSymbol &properties, Placement &placement)
    : properties(properties),
      textVertexBuffer(textVertexBuffer),
      iconVertexBuffer(iconVertexBuffer),
      triangleElementsBuffer(triangleElementsBuffer),
      placement(placement),
      text_vertex_start(textVertexBuffer.index()),
      icon_vertex_start(iconVertexBuffer.index()),
      triangle_elements_start(triangleElementsBuffer.index()) {}

void SymbolBucket::render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc,
                       const Tile::ID &id) {
   painter.renderSymbol(*this, layer_desc, id);
}

bool SymbolBucket::hasData() const {
    return hasTextData() || hasIconData();
}


bool SymbolBucket::hasTextData() const {
   return !triangleGroups.empty();
}

bool SymbolBucket::hasIconData() const {
    return icon_vertex_end > 0;
}

void SymbolBucket::addGlyph(uint64_t tileid, const std::string stackname,
                          const std::u32string &string, const FontStack &fontStack,
                          GlyphAtlas &glyphAtlas, GlyphPositions &face) {
    const std::map<uint32_t, SDFGlyph> &sdfs = fontStack.getSDFs();
    // Loop through all characters and add glyph to atlas, positions.
    for (uint32_t chr : string) {
        auto sdf_it = sdfs.find(chr);
        if (sdf_it != sdfs.end()) {
            const SDFGlyph& sdf = sdf_it->second;
            const Rect<uint16_t> rect = glyphAtlas.addGlyph(tileid, stackname, sdf);
            face.emplace(chr, Glyph{rect, sdf.metrics});
        }
    }
}

void SymbolBucket::addFeatures(const VectorTileLayer &layer, const FilterExpression &filter,
                               const Tile::ID &id, SpriteAtlas &spriteAtlas, GlyphAtlas &glyphAtlas,
                               GlyphStore &glyphStore) {
    const bool text = properties.text.field.size();
    const bool icon = properties.icon.image.size();

    util::utf8_to_utf32 ucs4conv;

    std::vector<std::pair<std::u32string, pbf>> labels;

    // Determine and load glyph ranges
    {
        std::set<GlyphRange> ranges;

        FilteredVectorTileLayer filtered_layer(layer, filter);
        for (const pbf &feature_pbf : filtered_layer) {
            VectorTileFeature feature {feature_pbf, layer};

            if (text) {
                std::string u8string = util::replaceTokens(properties.text.field, feature.properties);

                auto& convert = std::use_facet<std::ctype<char>>(std::locale());
                if (properties.text.transform == TextTransformType::Uppercase) {
                    convert.toupper(&u8string[0], &u8string[0] + u8string.size());
                } else if (properties.text.transform == TextTransformType::Lowercase) {
                    convert.tolower(&u8string[0], &u8string[0] + u8string.size());
                }

                std::u32string string = ucs4conv.convert(u8string);

                if (string.size()) {
                    // Loop through all characters of this text and collect unique codepoints.
                    for (char32_t chr : string) {
                        ranges.insert(getGlyphRange(chr));
                    }

                    labels.emplace_back(string, feature.geometry);
                }
            }

            // TODO: refactor for simultaneous placement
            // TODO: refactor to use quads instead of GL_POINTS
            if (icon) {
                std::string field = util::replaceTokens(properties.icon.image, feature.properties);

                // TODO: remove hardcoded size 12.
                const Rect<uint16_t> rect = spriteAtlas.getIcon(12, field);
                const uint16_t tx = rect.x + rect.w / 2;
                const uint16_t ty = rect.y + rect.h / 2;

                Geometry::command cmd;
                pbf geom = feature.geometry;
                Geometry geometry(geom);
                int32_t x, y;
                while ((cmd = geometry.next(x, y)) != Geometry::end) {
                    if (cmd == Geometry::move_to) {
                        iconVertexBuffer.add(x, y, tx, ty);
                    } else {
                        Log::Warning(Event::ParseTile, "other command than move_to in icon geometry");
                    }
                }

                icon_vertex_end = iconVertexBuffer.index();
            }
        }

        glyphStore.waitForGlyphRanges(properties.text.font, ranges);
    }


    float horizontalAlign = 0.5;
    if (properties.text.horizontal_align == TextHorizontalAlignType::Right) horizontalAlign = 1;
    else if (properties.text.horizontal_align == TextHorizontalAlignType::Left) horizontalAlign = 0;

    float verticalAlign = 0.5;
    if (properties.text.vertical_align == TextVerticalAlignType::Bottom) verticalAlign = 1;
    else if (properties.text.vertical_align == TextVerticalAlignType::Top) verticalAlign = 0;



    // Create a copy!
    const FontStack &fontStack = glyphStore.getFontStack(properties.text.font);
    GlyphPositions face;


    // Shape and place all labels.
    for (const std::pair<std::u32string, pbf> &label : labels) {

        // Shape labels.
        const Shaping shaping = fontStack.getShaping(label.first, properties.text.max_width,
               properties.text.line_height, horizontalAlign,
               verticalAlign, properties.text.letter_spacing);

        addGlyph(id.to_uint64(), properties.text.font, label.first, fontStack, glyphAtlas, face);

        // Place labels.
        addFeature(label.second, face, shaping);
    }
}

void SymbolBucket::addFeature(const pbf &geom_pbf, const GlyphPositions &face,
                              const Shaping &shaping) {
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
               placement.addFeature(*this, line, properties, face, shaping);
               line.clear();
           }
       }
       line.emplace_back(x, y);
   }
   if (line.size()) {
       placement.addFeature(*this, line, properties, face, shaping);
   }
}

void SymbolBucket::addGlyphs(const PlacedGlyphs &glyphs, float placementZoom,
                             PlacementRange placementRange, float zoom) {
    placementZoom += zoom;

    for (const PlacedGlyph &glyph : glyphs) {
        const auto &tl = glyph.tl;
        const auto &tr = glyph.tr;
        const auto &bl = glyph.bl;
        const auto &br = glyph.br;
        const auto &tex = glyph.tex;
        const auto &angle = glyph.angle;

        float minZoom = util::max(static_cast<float>(zoom + log(glyph.glyphBox.minScale) / log(2)),
                                  placementZoom);
        float maxZoom =
            util::min(static_cast<float>(zoom + log(glyph.glyphBox.maxScale) / log(2)), 25.0f);
        const auto &glyphAnchor = glyph.glyphBox.anchor;

        if (maxZoom <= minZoom)
            continue;

        // Lower min zoom so that while fading out the label
        // it can be shown outside of collision-free zoom levels
        if (minZoom == placementZoom) {
            minZoom = 0;
        }

        const int glyph_vertex_length = 4;

        if (!triangleGroups.size() ||
            (triangleGroups.back().vertex_length + glyph_vertex_length > 65535)) {
            // Move to a new group because the old one can't hold the geometry.
            triangleGroups.emplace_back();
        }

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        triangle_group_type &triangleGroup = triangleGroups.back();
        uint32_t triangleIndex = triangleGroup.vertex_length;

        // coordinates (2 triangles)
        textVertexBuffer.add(glyphAnchor.x, glyphAnchor.y, tl.x, tl.y, tex.x, tex.y, angle, minZoom,
                             placementRange, maxZoom, placementZoom);
        textVertexBuffer.add(glyphAnchor.x, glyphAnchor.y, tr.x, tr.y, tex.x + tex.w, tex.y, angle,
                             minZoom, placementRange, maxZoom, placementZoom);
        textVertexBuffer.add(glyphAnchor.x, glyphAnchor.y, bl.x, bl.y, tex.x, tex.y + tex.h, angle,
                             minZoom, placementRange, maxZoom, placementZoom);
        textVertexBuffer.add(glyphAnchor.x, glyphAnchor.y, br.x, br.y, tex.x + tex.w, tex.y + tex.h,
                             angle, minZoom, placementRange, maxZoom, placementZoom);

        // add the two triangles, referencing the four coordinates we just inserted.
        triangleElementsBuffer.add(triangleIndex + 0, triangleIndex + 1, triangleIndex + 2);
        triangleElementsBuffer.add(triangleIndex + 1, triangleIndex + 2, triangleIndex + 3);

        triangleGroup.vertex_length += glyph_vertex_length;
        triangleGroup.elements_length += 2;
    }
}

void SymbolBucket::drawGlyphs(TextShader &shader) {
   char *vertex_index = BUFFER_OFFSET(text_vertex_start * textVertexBuffer.itemSize);
   char *elements_index =
       BUFFER_OFFSET(triangle_elements_start * triangleElementsBuffer.itemSize);
   for (triangle_group_type &group : triangleGroups) {
       group.array.bind(shader, textVertexBuffer, triangleElementsBuffer, vertex_index);
       glDrawElements(GL_TRIANGLES, group.elements_length * 3, GL_UNSIGNED_SHORT, elements_index);
       vertex_index += group.vertex_length * textVertexBuffer.itemSize;
       elements_index += group.elements_length * triangleElementsBuffer.itemSize;
   }
}

void SymbolBucket::drawIcons(IconShader& shader) {
   char *vertex_index = BUFFER_OFFSET(icon_vertex_start * iconVertexBuffer.itemSize);
   array.bind(shader, iconVertexBuffer, vertex_index);
   glDrawArrays(GL_POINTS, 0, (GLsizei)(icon_vertex_end - icon_vertex_start));
}

void SymbolBucket::drawIcons(DotShader& shader) {
   char *vertex_index = BUFFER_OFFSET(icon_vertex_start * iconVertexBuffer.itemSize);
   array.bind(shader, iconVertexBuffer, vertex_index);
   glDrawArrays(GL_POINTS, 0, (GLsizei)(icon_vertex_end - icon_vertex_start));
}

}
