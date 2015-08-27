#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/renderer/gl_config.hpp>

#include <cstdio>

using namespace mbgl;

IconShader::IconShader()
    : Shader(
        "icon",
        shaders[ICON_SHADER].vertex,
        shaders[ICON_SHADER].fragment,
        [] (GLuint p) {
            MBGL_CHECK_ERROR(glBindAttribLocation(p, 0, "a_pos"));
            MBGL_CHECK_ERROR(glBindAttribLocation(p, 1, "a_offset"));
            MBGL_CHECK_ERROR(glBindAttribLocation(p, 2, "a_data1"));
            MBGL_CHECK_ERROR(glBindAttribLocation(p, 3, "a_data2"));
        }
    ) {
    a_pos = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_pos"));
    a_offset = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_offset"));
    a_data1 = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_data1"));
    a_data2 = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_data2"));
}

void IconShader::bind(gl::Config& config, char *offset) {
    config.enableVertexAttributes({ a_pos, a_offset, a_data1, a_data2 });

    const int stride = 16;
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_SHORT, false, stride, offset + 0));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_offset, 2, GL_SHORT, false, stride, offset + 4));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_data1, 4, GL_UNSIGNED_BYTE, false, stride, offset + 8));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_data2, 4, GL_UNSIGNED_BYTE, false, stride, offset + 12));
}
