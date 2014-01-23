#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/geometry/fill_buffer.hpp>
#include <llmr/geometry/geometry.hpp>

#include <llmr/renderer/painter.hpp>
#include <llmr/style/style.hpp>

#include <llmr/platform/gl.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#include <cassert>

struct geometry_too_long_exception : std::exception {};

using namespace llmr;

FillBucket::FillBucket(FillBuffer& buffer)
    : buffer(buffer),
      vertex_start(buffer.vertex_length()),
      elements_start(buffer.elements_length()),
      length(0) {
}

void FillBucket::addGeometry(pbf& geom) {
    std::vector<std::vector<std::pair<int16_t, int16_t>>> lines;

    {
        std::vector<std::pair<int16_t, int16_t>> line;
        Geometry::command cmd;
        int32_t x, y;
        Geometry geometry(geom);
        while ((cmd = geometry.next(x, y)) != Geometry::end) {
            if (cmd == Geometry::move_to) {
                if (line.size()) {
                    lines.push_back(line);
                    line.clear();
                }
            }
            line.emplace_back(x, y);
        }
        if (line.size()) {
            lines.push_back(line);
        }
    }

    for (const std::vector<std::pair<int16_t, int16_t>>& line : lines) {
        uint32_t vertex_start = buffer.vertex_length();

        buffer.addDegenerate();
        for (const std::pair<int16_t, int16_t>& coord : line) {
            buffer.addCoordinate(coord.first, coord.second);
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
}

void FillBucket::drawElements(int32_t attrib) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * 2 * sizeof(uint16_t));
    char *elements_index = BUFFER_OFFSET(elements_start * 3 * sizeof(uint16_t));
    buffer.bind();
    for (const auto& group : groups) {
        glVertexAttribPointer(attrib, 2, GL_SHORT, GL_FALSE, 0, vertex_index);
        glDrawElements(GL_TRIANGLES, group.elements_length * 3, GL_UNSIGNED_SHORT, elements_index);
        vertex_index += group.vertex_length * 2 * sizeof(uint16_t);
        elements_index += group.elements_length * 3 * sizeof(uint16_t);
    }
}

void FillBucket::drawVertices(int32_t attrib) {
    // Draw the entire line
    char *vertex_index = BUFFER_OFFSET(vertex_start * 2 * sizeof(uint16_t));
    glVertexAttribPointer(attrib, 2, GL_SHORT, GL_FALSE, 0, vertex_index);
    glDrawArrays(GL_LINE_STRIP, 0, length);
}

void FillBucket::render(Painter& painter) {
    // TODO: obtain the correct style information for this layer
    FillProperties properties;
    properties.fill_color = {{ 0, 0, 1, 0.5 }};
    properties.stroke_color = properties.fill_color;

    painter.renderFill(*this, properties);
}
