#include <llmr/renderer/text_bucket.hpp>
#include <llmr/geometry/text_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/geometry.hpp>

#include <llmr/renderer/painter.hpp>
#include <llmr/shader/text_shader.hpp>
#include <llmr/style/style.hpp>
#include <llmr/map/vector_tile.hpp>
#include <llmr/text/placement.hpp>

#include <llmr/util/math.hpp>
#include <llmr/platform/gl.hpp>

#include <iostream>

#define BUFFER_OFFSET(i) ((char *) nullptr + (i))

#include <cassert>

using namespace llmr;

TextBucket::TextBucket(
    const std::shared_ptr<TextVertexBuffer> &vertexBuffer,
    const std::shared_ptr<TriangleElementsBuffer> &triangleElementsBuffer,
    const BucketDescription &bucket_desc, Placement &placement)
    : geom_desc(bucket_desc.geometry),
      vertexBuffer(vertexBuffer),
      triangleElementsBuffer(triangleElementsBuffer),
      placement(placement),
      vertex_start(vertexBuffer->index()),
      triangle_elements_start(triangleElementsBuffer->index()) {}

void TextBucket::addGlyphs(const PlacedGlyphs &glyphs, float placementZoom,
                           PlacementRange placementRange, float zoom) {
    placementZoom += zoom;

    for (const PlacedGlyph &glyph : glyphs) {
        const auto &tl = glyph.tl;
        const auto &tr = glyph.tr;
        const auto &bl = glyph.bl;
        const auto &br = glyph.br;
        const auto &tex = glyph.tex;
        const auto &width = glyph.width;
        const auto &height = glyph.height;
        const auto &angle = glyph.angle;

        float minZoom = util::max(
            static_cast<float>(zoom + log(glyph.glyphBox.minScale) / log(2)),
            placementZoom);
        float maxZoom = util::min(
            static_cast<float>(zoom + log(glyph.glyphBox.maxScale) / log(2)),
            25.0f);
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
            (triangleGroups.back().vertex_length + glyph_vertex_length >
             65535)) {
            // Move to a new group because the old one can't hold the geometry.
            triangleGroups.emplace_back();
        }

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        triangle_group_type &triangleGroup = triangleGroups.back();
        uint32_t triangleIndex = triangleGroup.vertex_length;

        // coordinates (2 triangles)
        vertexBuffer->add(glyphAnchor.x, glyphAnchor.y, tl.x, tl.y, tex.x,
                          tex.y, angle, minZoom, placementRange, maxZoom,
                          placementZoom);
        vertexBuffer->add(glyphAnchor.x, glyphAnchor.y, tr.x, tr.y,
                          tex.x + width, tex.y, angle, minZoom, placementRange,
                          maxZoom, placementZoom);
        vertexBuffer->add(glyphAnchor.x, glyphAnchor.y, bl.x, bl.y, tex.x,
                          tex.y + height, angle, minZoom, placementRange,
                          maxZoom, placementZoom);
        vertexBuffer->add(glyphAnchor.x, glyphAnchor.y, br.x, br.y,
                          tex.x + width, tex.y + height, angle, minZoom,
                          placementRange, maxZoom, placementZoom);

        // add the two triangles, referencing the four coordinates we just
        // inserted.
        triangleElementsBuffer->add(triangleIndex + 0, triangleIndex + 1,
                                    triangleIndex + 1);
        triangleElementsBuffer->add(triangleIndex + 1, triangleIndex + 2,
                                    triangleIndex + 3);

        triangleGroup.vertex_length += glyph_vertex_length;
    }
};

void TextBucket::addFeature(const VectorTileFeature &feature,
                            const IndexedFaceGlyphPositions &faces,
                            const std::map<Value, Shaping> &shapings) {
    auto it_prop = feature.properties.find(geom_desc.text_field);
    if (it_prop == feature.properties.end()) {
        // feature does not have the correct property
        return;
    }
    const Value &value = it_prop->second;

    auto it_shaping = shapings.find(value);
    if (it_shaping == shapings.end()) {
        // we lack shaping information for this label
        return;
    }
    const Shaping &shaping = it_shaping->second;

    // std::cerr << "we have shaping for " << value << std::endl;

    // Decode all lines.
    std::vector<Coordinate> line;
    Geometry::command cmd;

    Coordinate coord;
    pbf geom = feature.geometry;
    Geometry geometry(geom);
    int32_t x, y;
    while ((cmd = geometry.next(x, y)) != Geometry::end) {
        if (cmd == Geometry::move_to) {
            if (!line.empty()) {
                placement.addFeature(*this, line, geom_desc, faces, shaping);
                line.clear();
            }
        }
        line.emplace_back(x, y);
    }
    if (line.size()) {
        placement.addFeature(*this, line, geom_desc, faces, shaping);
    }
}

void TextBucket::render(Painter &painter, const std::string &layer_name,
                        const Tile::ID &id) {
    painter.renderText(*this, layer_name, id);
}

bool TextBucket::empty() const { return triangleGroups.empty(); }

void TextBucket::drawGlyphs(TextShader &shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer->itemSize);
    char *elements_index =
        BUFFER_OFFSET(triangle_elements_start * triangleElementsBuffer->itemSize);
    for (triangle_group_type &group : triangleGroups) {
        group.array.bind(shader, *vertexBuffer, *triangleElementsBuffer, vertex_index);
        glDrawElements(GL_TRIANGLES, group.elements_length * 3, GL_UNSIGNED_SHORT, elements_index);
        vertex_index += group.vertex_length * vertexBuffer->itemSize;
        elements_index += group.elements_length * triangleElementsBuffer->itemSize;
    }
}
