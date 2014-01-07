#include <llmr/renderer/shader-line.hpp>
#include <llmr/shader/shaders.h>
#include <llmr/platform/gl.hpp>

#include <cstdio>

using namespace llmr;

LineShader::LineShader()
    : Shader(
        llmr_shaders[LINE_SHADER].vertex,
        llmr_shaders[LINE_SHADER].fragment
    ),
    a_pos(42) {
    if (!valid) {
        fprintf(stderr, "invalid line shader\n");
        return;
    }

    GLint pos = glGetAttribLocation(program, "a_pos");
    fprintf(stderr, "a_pos: %d\n", pos);

    a_pos = glGetAttribLocation(program, "a_pos");
    attributes.push_back(a_pos);

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
}
