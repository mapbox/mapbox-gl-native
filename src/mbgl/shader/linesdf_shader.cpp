#include <mbgl/shader/linesdf_shader.hpp>
#include <mbgl/shader/linesdfpattern.vertex.hpp>
#include <mbgl/shader/linesdfpattern.fragment.hpp>
#include <mbgl/gl/gl.hpp>

using namespace mbgl;
using namespace shaders::linesdfpattern;

LineSDFShader::LineSDFShader(gl::ObjectStore& store)
    : Shader(::name, ::vertex, ::fragment, store)
    , a_data(MBGL_CHECK_ERROR(glGetAttribLocation(getID(), "a_data"))) {
}

void LineSDFShader::bind(GLbyte* offset) {
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_pos));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 8, offset + 0));

    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_data));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_data, 4, GL_UNSIGNED_BYTE, false, 8, offset + 4));
}
