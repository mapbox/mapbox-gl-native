#include <mbgl/shader/outlinepattern_shader.hpp>
#include <mbgl/shader/outlinepattern.vertex.hpp>
#include <mbgl/shader/outlinepattern.fragment.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

OutlinePatternShader::OutlinePatternShader(gl::ObjectStore& store, Defines defines)
    : Shader(shaders::outlinepattern::name,
             shaders::outlinepattern::vertex,
             shaders::outlinepattern::fragment,
             store, defines) {
}

void OutlinePatternShader::bind(GLbyte *offset) {
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_pos));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset));
}

} // namespace mbgl
