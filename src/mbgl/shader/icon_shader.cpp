#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/shader/icon.vertex.hpp>
#include <mbgl/shader/icon.fragment.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

IconShader::IconShader(gl::ObjectStore& store, Defines defines)
    : Shader(shaders::icon::name,
             shaders::icon::vertex,
             shaders::icon::fragment,
             store, defines) {
}

void IconShader::bind(GLbyte* offset) {
    const GLsizei stride = 16;

    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_pos));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_SHORT, false, stride, offset + 0));

    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_offset));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_offset, 2, GL_SHORT, false, stride, offset + 4));

    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_texture_pos));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_texture_pos, 2, GL_UNSIGNED_SHORT, false, stride, offset + 8));

    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_data));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_data, 4, GL_UNSIGNED_BYTE, false, stride, offset + 12));
}

} // namespace mbgl
