#include <mbgl/shader/linepattern_shader.hpp>
#include <mbgl/shader/linepattern.vertex.hpp>
#include <mbgl/shader/linepattern.fragment.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

LinepatternShader::LinepatternShader(gl::ObjectStore& store, Defines defines)
    : Shader(shaders::linepattern::name,
             shaders::linepattern::vertex,
             shaders::linepattern::fragment,
             store, defines) {
}

void LinepatternShader::bind(GLbyte* offset) {
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_pos));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 8, offset + 0));

    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_data));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_data, 4, GL_UNSIGNED_BYTE, false, 8, offset + 4));
}

} // namespace mbgl
