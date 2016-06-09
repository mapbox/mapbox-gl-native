#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/shader/fill.vertex.hpp>
#include <mbgl/shader/fill.fragment.hpp>
#include <mbgl/gl/gl.hpp>

#include <cstdio>

using namespace mbgl;

PlainShader::PlainShader()
    : Shader("fill", shaders::fill::vertex, shaders::fill::fragment) {
}

void PlainShader::bind(GLbyte* offset) {
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_pos));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset));
}
