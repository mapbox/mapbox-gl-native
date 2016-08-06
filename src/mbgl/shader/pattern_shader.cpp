#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/pattern.vertex.hpp>
#include <mbgl/shader/pattern.fragment.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

PatternShader::PatternShader(gl::ObjectStore& store, Defines defines)
    : Shader(shaders::pattern::name,
             shaders::pattern::vertex,
             shaders::pattern::fragment,
             store, defines) {
}

void PatternShader::bind(GLbyte *offset) {
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_pos));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset));
}

} // namespace mbgl
