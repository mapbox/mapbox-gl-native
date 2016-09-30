#include <mbgl/shader/texture_rect_vertex.hpp>
#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

void TextureRectVertex::bind(const int8_t* offset) {
    static_assert(sizeof(TextureRectVertex) == 16, "expected TextureRectVertex size");

    MBGL_BIND_VERTEX_ATTRIBUTE(TextureRectVertex, a_pos, offset);
    MBGL_BIND_VERTEX_ATTRIBUTE(TextureRectVertex, a_offset, offset);
    MBGL_BIND_VERTEX_ATTRIBUTE(TextureRectVertex, a_texture_pos, offset);
    MBGL_BIND_VERTEX_ATTRIBUTE(TextureRectVertex, a_data, offset);
}

} // namespace mbgl
