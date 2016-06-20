#include <mbgl/shader/linepattern_shader.hpp>
#include <mbgl/shader/linepattern.vertex.hpp>
#include <mbgl/shader/linepattern.fragment.hpp>
#include <mbgl/gl/gl.hpp>

using namespace mbgl;
using namespace shaders::linepattern;

LinepatternShader::LinepatternShader(gl::ObjectStore& store)
    : Shader(::name, ::vertex, ::fragment, store)
    , a_data(MBGL_CHECK_ERROR(glGetAttribLocation(getID(), "a_data"))) {
}

void LinepatternShader::bind(GLbyte* offset) {
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_pos));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 8, offset + 0));

    MBGL_CHECK_ERROR(glEnableVertexAttribArray(a_data));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_data, 4, GL_UNSIGNED_BYTE, false, 8, offset + 4));
}
