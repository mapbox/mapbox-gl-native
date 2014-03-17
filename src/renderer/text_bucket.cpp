#include <llmr/renderer/text_bucket.hpp>
#include <llmr/geometry/text_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/geometry.hpp>

#include <llmr/renderer/painter.hpp>
#include <llmr/shader/text_shader.hpp>
#include <llmr/style/style.hpp>
#include <llmr/map/vector_tile.hpp>

#include <llmr/util/math.hpp>
#include <llmr/platform/gl.hpp>

#include <iostream>

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

#include <cassert>

using namespace llmr;

TextBucket::TextBucket(const std::shared_ptr<TextVertexBuffer>& vertexBuffer,
                       const std::shared_ptr<TriangleElementsBuffer>& triangleElementsBuffer,
                       const BucketDescription& bucket_desc)
    : geometry(bucket_desc.geometry),
      vertexBuffer(vertexBuffer),
      triangleElementsBuffer(triangleElementsBuffer),
      vertex_start(vertexBuffer->index()),
      triangle_elements_start(triangleElementsBuffer->index()) {
}

void TextBucket::addFeature(const VectorTileFeature& feature,
                            const std::vector<const VectorTileFace *>& faces,
                            const std::map<Value, std::vector<VectorTilePlacement>>& shaping) {
    auto it_prop = feature.properties.find(geometry.text_field);
    if (it_prop == feature.properties.end()) {
        // feature does not have the correct property
        return;
    }
    const Value& value = it_prop->second;


    auto it_shaping = shaping.find(value);
    if (it_shaping == shaping.end()) {
        // we lack shaping information for this label
        return;
    }
    const std::vector<VectorTilePlacement>& placements = it_shaping->second;

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
                addLabel(line, faces, placements);
                line.clear();
            }
        }
        line.emplace_back(x, y);
    }
    if (line.size()) {
        addLabel(line, faces, placements);
    }
}

void TextBucket::addLabel(const std::vector<Coordinate>& line,
                          const std::vector<const VectorTileFace *>& faces,
                          const std::vector<VectorTilePlacement>& placements) {
    // TODO
}

void TextBucket::render(Painter& painter, const std::string& layer_name, const Tile::ID& id) {
    painter.renderText(*this, layer_name, id);
}

bool TextBucket::empty() const {
    return triangleGroups.empty();
}

void TextBucket::drawGlyphs(TextShader& shader) {
    // noop
}
