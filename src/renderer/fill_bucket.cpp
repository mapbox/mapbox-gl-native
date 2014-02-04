#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/geometry/fill_buffer.hpp>
#include <llmr/geometry/geometry.hpp>

#include <llmr/renderer/painter.hpp>
#include <llmr/style/style.hpp>
#include <llmr/map/vector_tile.hpp>

#include <llmr/platform/gl.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#include <cassert>

struct geometry_too_long_exception : std::exception {};

using namespace llmr;

FillBucket::FillBucket(const std::shared_ptr<FillBuffer>& buffer, const BucketDescription& bucket_desc)
    : geom_desc(bucket_desc.geometry),
      buffer(buffer),
      vertex_start(buffer->vertex_length()),
      elements_start(buffer->elements_length()),
      length(0) {
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
    // Alias this.
    FillBuffer& buffer = *this->buffer;

    uint32_t vertex_start = buffer.vertex_length();

    buffer.addDegenerate();
    for (const Coordinate& coord : line) {
        buffer.addCoordinate(coord.x, coord.y);
    }

    uint32_t vertex_end = buffer.vertex_length();

    if (vertex_end - vertex_start > 65535) {
        throw geometry_too_long_exception();
    }

    FillBucket& index = *this;
    if (!index.groups.size()) {
        index.groups.emplace_back();
    }

    uint32_t vertex_count = vertex_end - vertex_start;
    index.length += vertex_count;

    if (index.groups.back().vertex_length + vertex_count > 65535) {
        // Move to a new group because the old one can't hold the geometry.
        index.groups.emplace_back();
    }

    group& group = index.groups.back();

    // We're generating triangle fans, so we always start with the first
    // coordinate in this polygon.
    // The first valid index that is not a degenerate.
    uint16_t firstIndex = group.vertex_length + 1;

    assert(firstIndex + vertex_count - 1 < 65536);

    uint32_t elements_start = buffer.elements_length();

    for (uint32_t i = 2; i < vertex_count; i++) {
        buffer.addElements(firstIndex, firstIndex + i - 1, firstIndex + i);
    }

    uint32_t elements_end = buffer.elements_length();
    uint32_t elements_count = elements_end - elements_start;
    group.vertex_length += vertex_count;
    group.elements_length += elements_count;

}

void FillBucket::drawElements(int32_t attrib) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * 2 * sizeof(int16_t));
    char *elements_index = BUFFER_OFFSET(elements_start * 3 * sizeof(int16_t));
    buffer->bind();
    for (const auto& group : groups) {
        glVertexAttribPointer(attrib, 2, GL_SHORT, GL_FALSE, 0, vertex_index);
        glDrawElements(GL_TRIANGLES, group.elements_length * 3, GL_UNSIGNED_SHORT, elements_index);
        vertex_index += group.vertex_length * 2 * sizeof(uint16_t);
        elements_index += group.elements_length * 3 * sizeof(uint16_t);
    }
}

void FillBucket::drawVertices(int32_t attrib) {
    // Draw the entire line
    char *vertex_index = BUFFER_OFFSET(vertex_start * 2 * sizeof(int16_t));
    buffer->bind();
    glVertexAttribPointer(attrib, 2, GL_SHORT, GL_FALSE, 0, vertex_index);
    glDrawArrays(GL_LINE_STRIP, 0, length);
}

void FillBucket::render(Painter& painter, const std::string& layer_name, const Tile::ID& id) {
    painter.renderFill(*this, layer_name, id);
}

uint32_t FillBucket::size() const {
    return length;
}

