#include <mbgl/shader/sdf_shader.hpp>
#include <mbgl/shader/sdf.vertex.hpp>
#include <mbgl/shader/sdf.fragment.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

SDFShader::SDFShader(gl::Context& context, Defines defines)
    : Shader(shaders::sdf::name,
             shaders::sdf::vertex,
             shaders::sdf::fragment,
             context, defines) {
}

void SDFShader::bind(int8_t* offset) {
    const int stride = 16;

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
