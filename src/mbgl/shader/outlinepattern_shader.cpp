#include <mbgl/shader/outlinepattern_shader.hpp>
#include <mbgl/shader/outlinepattern.vertex.hpp>
#include <mbgl/shader/outlinepattern.fragment.hpp>
#include <mbgl/gl/gl.hpp>

using namespace mbgl;
using namespace shaders::outlinepattern;

OutlinePatternShader::OutlinePatternShader(gl::ObjectStore& store)
    : Shader(::name, ::vertex, ::fragment, store) {
}

void OutlinePatternShader::bind(GLbyte *offset) {
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_pos));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset));
}
