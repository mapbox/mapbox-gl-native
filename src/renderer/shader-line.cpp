#include <llmr/renderer/shader-line.hpp>
#include <llmr/shader/shaders.hpp>
#include <llmr/platform/gl.hpp>

#include <cstdio>

using namespace llmr;

LineShader::LineShader()
    : Shader(
        shaders[LINE_SHADER].vertex,
        shaders[LINE_SHADER].fragment
    ) {
    if (!valid) {
        fprintf(stderr, "invalid line shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");
    a_extrude = glGetAttribLocation(program, "a_extrude");
    a_linesofar = glGetAttribLocation(program, "a_linesofar");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_exmatrix = glGetUniformLocation(program, "u_exmatrix");
    u_linewidth = glGetUniformLocation(program, "u_linewidth");
    u_color = glGetUniformLocation(program, "u_color");
    u_debug = glGetUniformLocation(program, "u_debug");
    u_ratio = glGetUniformLocation(program, "u_ratio");
    u_dasharray = glGetUniformLocation(program, "u_dasharray");
    u_point = glGetUniformLocation(program, "u_point");
    u_gamma = glGetUniformLocation(program, "u_gamma");
}

void LineShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 8, offset);

    glEnableVertexAttribArray(a_extrude);
    glVertexAttribPointer(a_extrude, 2, GL_BYTE, false, 8, offset + 6);

    glEnableVertexAttribArray(a_linesofar);
    glVertexAttribPointer(a_linesofar, 1, GL_SHORT, false, 8, offset + 4);
}