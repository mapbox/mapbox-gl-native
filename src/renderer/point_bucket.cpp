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
                         const BucketDescription& bucket_desc)
    : geometry(bucket_desc.geometry),
      vertexBuffer(vertexBuffer),
      vertex_start(vertexBuffer->index()) {
}

void PointBucket::addGeometry(pbf& geom) {
    Geometry::command cmd;
    Geometry geometry(geom);
    int32_t x, y;
    while ((cmd = geometry.next(x, y)) != Geometry::end) {
        if (cmd == Geometry::move_to) {
            vertexBuffer->add(x, y);
        } else {
            fprintf(stderr, "other command than move_to in point geometry\n");
        }
    }

    vertex_end = vertexBuffer->index();
}

void PointBucket::render(Painter& painter, const std::string& layer_name, const Tile::ID& id) {
    painter.renderPoint(*this, layer_name, id);
}

bool PointBucket::hasPoints() const {
    return vertex_end > 0;
}

void PointBucket::drawPoints(PointShader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer->itemSize);
    array.bind(shader, *vertexBuffer, vertex_index);
    glDrawArrays(GL_POINTS, 0, (GLsizei)(vertex_end - vertex_start));
}
