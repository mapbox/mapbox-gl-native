#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/geometry/fill_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/geometry.hpp>

#include <llmr/renderer/painter.hpp>
#include <llmr/style/style.hpp>
#include <llmr/map/vector_tile.hpp>

#include <llmr/platform/gl.hpp>

#include <cassert>

struct geometry_too_long_exception : std::exception {};

using namespace llmr;

FillBucket::FillBucket(const std::shared_ptr<FillVertexBuffer>& vertexBuffer,
                       const std::shared_ptr<TriangleElementsBuffer>& triangleElementsBuffer,
                       const std::shared_ptr<LineElementsBuffer>& lineElementsBuffer,
                       const BucketDescription& bucket_desc)
    : geom_desc(bucket_desc.geometry),
      vertexBuffer(vertexBuffer),
      triangleElementsBuffer(triangleElementsBuffer),
      lineElementsBuffer(lineElementsBuffer),
      vertex_start(vertexBuffer->index()),
      triangle_elements_start(triangleElementsBuffer->index()),
      line_elements_start(lineElementsBuffer->index()) {
}

void FillBucket::addGeometry(pbf& geom) {
    std::vector<Coordinate> line;
    Geometry::command cmd;

    Coordinate coord;
    Geometry geometry(geom);
    int32_t x, y;
    while ((cmd = geometry.next(x, y)) != Geometry::end) {
        if (cmd == Geometry::move_to) {
            if (line.size()) {
                addGeometry(line);
                line.clear();
            }
        }
        line.emplace_back(x, y);
    }
    if (line.size()) {
        addGeometry(line);
    }
}

void FillBucket::addGeometry(const std::vector<Coordinate>& line) {
    uint32_t vertex_start = vertexBuffer->index();

    size_t length = line.size();

    // Don't add the first vertex twice.
    if (line.front() == line.back()) --length;

    for (size_t i = 0; i < length; i++) {
        const Coordinate& coord = line[i];
        vertexBuffer->add(coord.x, coord.y);
    }

    size_t vertex_end = vertexBuffer->index();

    size_t vertex_count = vertex_end - vertex_start;
    if (vertex_count > 65536) {
        throw geometry_too_long_exception();
    }

    {
        if (!triangleGroups.size() || (triangleGroups.back().vertex_length + vertex_count > 65535)) {
            // Move to a new group because the old one can't hold the geometry.
            triangleGroups.emplace_back();
        }

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        triangle_group_type& group = triangleGroups.back();
        uint32_t index = group.vertex_length;
        for (size_t i = 2; i < vertex_count; ++i) {
            triangleElementsBuffer->add(index, index + i - 1, index + i);
        }

        group.vertex_length += vertex_count;
        group.elements_length += (vertex_count - 2);
    }

    {
        if (!lineGroups.size() || (lineGroups.back().vertex_length + vertex_count > 65535)) {
            // Move to a new group because the old one can't hold the geometry.
            lineGroups.emplace_back();
        }

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        line_group_type& group = lineGroups.back();
        uint32_t index = group.vertex_length;
        for (size_t i = 1; i < vertex_count; ++i) {
            lineElementsBuffer->add(index + i - 1, index + i);
        }

        // Add a line from the last vertex to the first vertex.
        lineElementsBuffer->add(index + vertex_count - 1, index);

        group.vertex_length += vertex_count;
        group.elements_length += vertex_count;
    }
}

void FillBucket::render(Painter& painter, const std::string& layer_name, const Tile::ID& id) {
    painter.renderFill(*this, layer_name, id);
}

bool FillBucket::empty() const {
    return triangleGroups.empty();
}

void FillBucket::drawElements(PlainShader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer->itemSize);
    char *elements_index = BUFFER_OFFSET(triangle_elements_start * triangleElementsBuffer->itemSize);
    for (triangle_group_type& group : triangleGroups) {
        group.array.bind(shader, *vertexBuffer, *triangleElementsBuffer, vertex_index);
        glDrawElements(GL_TRIANGLES, group.elements_length * 3, GL_UNSIGNED_SHORT, elements_index);
        vertex_index += group.vertex_length * vertexBuffer->itemSize;
        elements_index += group.elements_length * triangleElementsBuffer->itemSize;
    }
}

void FillBucket::drawVertices(OutlineShader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer->itemSize);
    char *elements_index = BUFFER_OFFSET(line_elements_start * lineElementsBuffer->itemSize);
    for (line_group_type& group : lineGroups) {
        group.array.bind(shader, *vertexBuffer, *lineElementsBuffer, vertex_index);
        glDrawElements(GL_LINES, group.elements_length * 2, GL_UNSIGNED_SHORT, elements_index);
        vertex_index += group.vertex_length * vertexBuffer->itemSize;
        elements_index += group.elements_length * lineElementsBuffer->itemSize;
    }
}
