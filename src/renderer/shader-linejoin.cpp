#include <llmr/renderer/shader-linejoin.hpp>
#include <llmr/shader/shaders.hpp>
#include <llmr/platform/gl.hpp>

#include <cstdio>

using namespace llmr;

LinejoinShader::LinejoinShader()
    : Shader(
        shaders[LINEJOIN_SHADER].vertex,
        shaders[LINEJOIN_SHADER].fragment
    ) {
    if (!valid) {
        fprintf(stderr, "invalid line shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_world = glGetUniformLocation(program, "u_world");
    u_linewidth = glGetUniformLocation(program, "u_linewidth");
    u_color = glGetUniformLocation(program, "u_color");
    u_size = glGetUniformLocation(program, "u_size");

    // fprintf(stderr, "LinejoinShader:\n");
    // fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    // fprintf(stderr, "    - u_world: %d\n", u_world);
    // fprintf(stderr, "    - u_linewidth: %d\n", u_linewidth);
    // fprintf(stderr, "    - u_color: %d\n", u_color);
    // fprintf(stderr, "    - u_size: %d\n", u_size);
}

void LinejoinShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    // Note: We're referring to the vertices in a line array, which are 8 bytes long!
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 8, offset);
}
