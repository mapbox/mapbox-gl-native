#include <mbgl/shader/line_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/renderer/gl_config.hpp>

#include <cstdio>

using namespace mbgl;

LineShader::LineShader()
    : Shader(
        "line",
        shaders[LINE_SHADER].vertex,
        shaders[LINE_SHADER].fragment,
        [] (GLuint p) {
            MBGL_CHECK_ERROR(glBindAttribLocation(p, 0, "a_pos"));
            MBGL_CHECK_ERROR(glBindAttribLocation(p, 1, "a_data"));
        }
    ) {
    a_pos = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_pos"));
    a_data = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_data"));
}

void LineShader::bind(gl::Config& config, char *offset) {
    config.enableVertexAttributes({ a_pos, a_data });

    MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 8, offset + 0));
    MBGL_CHECK_ERROR(glVertexAttribPointer(a_data, 4, GL_BYTE, false, 8, offset + 4));
}
