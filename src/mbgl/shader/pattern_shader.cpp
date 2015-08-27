#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/renderer/gl_config.hpp>

#include <cstdio>

using namespace mbgl;

PatternShader::PatternShader()
    : Shader(
        "pattern",
        shaders[PATTERN_SHADER].vertex,
        shaders[PATTERN_SHADER].fragment,
        [] (GLuint p) {
            MBGL_CHECK_ERROR(glBindAttribLocation(p, 0, "a_pos"));
        }
    ) {
    a_pos = MBGL_CHECK_ERROR(glGetAttribLocation(program, "a_pos"));
}

void PatternShader::bind(gl::Config& config, char *offset) {
    config.enableVertexAttributes({ a_pos });

    MBGL_CHECK_ERROR(glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset));
}
