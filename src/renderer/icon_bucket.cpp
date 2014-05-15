#include <llmr/renderer/icon_bucket.hpp>
#include <llmr/geometry/icon_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/geometry.hpp>

#include <llmr/renderer/painter.hpp>
#include <llmr/style/style.hpp>
#include <llmr/map/vector_tile.hpp>

#include <llmr/platform/gl.hpp>

#include <cassert>

struct geometry_too_long_exception : std::exception {};

using namespace llmr;

IconBucket::IconBucket(IconVertexBuffer& vertexBuffer,
                         const BucketDescription& bucket_desc)
    : geometry(bucket_desc.geometry),
      vertexBuffer(vertexBuffer),
      vertex_start(vertexBuffer.index()) {
}

void IconBucket::addGeometry(pbf& geom) {
    Geometry::command cmd;
    Geometry geometry(geom);
    int32_t x, y;
    while ((cmd = geometry.next(x, y)) != Geometry::end) {
        if (cmd == Geometry::move_to) {
            vertexBuffer.add(x, y);
        } else {
            fprintf(stderr, "other command than move_to in icon geometry\n");
        }
    }

    vertex_end = vertexBuffer.index();
}

void IconBucket::render(Painter& painter, const std::string& layer_name, const Tile::ID& id) {
    painter.renderIcon(*this, layer_name, id);
}

bool IconBucket::hasData() const {
    return vertex_end > 0;
}

void IconBucket::drawIcons(IconShader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer.itemSize);
    array.bind(shader, vertexBuffer, vertex_index);
    glDrawArrays(GL_POINTS, 0, (GLsizei)(vertex_end - vertex_start));
}

void IconBucket::drawIcons(DotShader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer.itemSize);
    array.bind(shader, vertexBuffer, vertex_index);
    glDrawArrays(GL_POINTS, 0, (GLsizei)(vertex_end - vertex_start));
}
