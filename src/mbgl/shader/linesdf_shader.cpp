#include <mbgl/shader/linesdf_shader.hpp>
#include <mbgl/shader/linesdfpattern.vertex.hpp>
#include <mbgl/shader/linesdfpattern.fragment.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

LineSDFShader::LineSDFShader(gl::ObjectStore& store, Defines defines)
    : Shader(shaders::linesdfpattern::name,
             shaders::linesdfpattern::vertex,
             shaders::linesdfpattern::fragment,
             store, defines) {
}

void LineSDFShader::bind(GLbyte* offset) {
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_pos));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 8, offset + 0));

    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_data));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_data, 4, GL_UNSIGNED_BYTE, false, 8, offset + 4));
}

} // namespace mbgl
