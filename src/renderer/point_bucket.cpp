#include <llmr/renderer/point_bucket.hpp>
#include <llmr/geometry/point_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/geometry.hpp>

#include <llmr/renderer/painter.hpp>
#include <llmr/style/style.hpp>
#include <llmr/map/vector_tile.hpp>

#include <llmr/platform/gl.hpp>

#include <cassert>

struct geometry_too_long_exception : std::exception {};

using namespace llmr;

PointBucket::PointBucket(const std::shared_ptr<PointVertexBuffer>& vertexBuffer,
                         const std::shared_ptr<PointElementsBuffer>& elementsBuffer,
                         const BucketDescription& bucket_desc)
    : geometry(bucket_desc.geometry),
      vertexBuffer(vertexBuffer),
      elementsBuffer(elementsBuffer),
      vertex_start(vertexBuffer->index()),
      elements_start(elementsBuffer->index())
{
}

void PointBucket::addGeometry(pbf& geom) {
    Coordinate point;

    Geometry geometry(geom);
    int32_t x, y;
    geometry.next(x, y);
    point.x = x;
    point.y = y;
    addGeometry(point);
}

typedef uint16_t PointElement;

void PointBucket::addGeometry(Coordinate& point) {
    vertexBuffer->add(point.x, point.y);

    {
        if (!groups.size() || (groups.back().vertex_length + 1 > 65535)) {
            groups.emplace_back();
        }

        group_type& group = groups.back();
        elementsBuffer->add(group.vertex_length + point);

        group.vertex_length++;
        group.elements_length++;
    }
}

void PointBucket::render(Painter& painter, const std::string& layer_name, const Tile::ID& id) {
    painter.renderPoint(*this, layer_name, id);
}

bool PointBucket::hasPoints() const {
    return !groups.empty();
}

void PointBucket::drawPoints(PointShader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer->itemSize);
    char *elements_index = BUFFER_OFFSET(elements_start * elementsBuffer->itemSize);
    for (group_type& group : groups) {
        group.array.bind(shader, *vertexBuffer, *elementsBuffer, vertex_index);
        glDrawElements(GL_POINTS, group.elements_length, GL_UNSIGNED_SHORT, elements_index);
        vertex_index += group.vertex_length * vertexBuffer->itemSize;
        elements_index += group.elements_length * elementsBuffer->itemSize;
    }
}
