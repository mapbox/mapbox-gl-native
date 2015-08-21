#include <mbgl/geometry/static_vertex_buffer.hpp>
#include <mbgl/platform/gl.hpp>

namespace mbgl {

StaticVertexBuffer::StaticVertexBuffer(std::initializer_list<std::pair<int16_t, int16_t>> init) {
    for (const auto& vertex : init) {
        vertex_type *vertices = static_cast<vertex_type *>(addElement());
        vertices[0] = vertex.first;
        vertices[1] = vertex.second;
    }
}

}
