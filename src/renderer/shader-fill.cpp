#include <llmr/renderer/shader-fill.hpp>
#include <llmr/shader/shaders.hpp>
#include <llmr/platform/gl.hpp>

#include <cstdio>

using namespace llmr;

FillShader::FillShader()
    : Shader(
        shaders[FILL_SHADER].vertex,
        shaders[FILL_SHADER].fragment
    ) {
    if (!valid) {
        fprintf(stderr, "invalid fill shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
}

void FillShader::bind_vertex(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset);
}
