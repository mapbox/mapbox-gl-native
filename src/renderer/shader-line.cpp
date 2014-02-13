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
    // a_linesofar = glGetAttribLocation(program, "a_linesofar");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_exmatrix = glGetUniformLocation(program, "u_exmatrix");
    u_linewidth = glGetUniformLocation(program, "u_linewidth");
    u_color = glGetUniformLocation(program, "u_color");
    u_ratio = glGetUniformLocation(program, "u_ratio");
    u_dasharray = glGetUniformLocation(program, "u_dasharray");
    u_debug = glGetUniformLocation(program, "u_debug");

    // fprintf(stderr, "LineShader:\n");
    // fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    // fprintf(stderr, "    - u_exmatrix: %d\n", u_exmatrix);
    // fprintf(stderr, "    - u_linewidth: %d\n", u_linewidth);
    // fprintf(stderr, "    - u_color: %d\n", u_color);
    // fprintf(stderr, "    - u_ratio: %d\n", u_ratio);
    // fprintf(stderr, "    - u_dasharray: %d\n", u_dasharray);
    // fprintf(stderr, "    - u_debug: %d\n", u_debug);
}

void LineShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 8, offset);

    // glEnableVertexAttribArray(a_linesofar);
    // glVertexAttribPointer(a_linesofar, 1, GL_SHORT, false, 8, offset + 6);

    glEnableVertexAttribArray(a_extrude);
    glVertexAttribPointer(a_extrude, 2, GL_BYTE, false, 8, offset + 4);
}

void LineShader::setExtrudeMatrix(const std::array<float, 16>& new_exmatrix) {
    if (exmatrix != new_exmatrix) {
        glUniformMatrix4fv(u_exmatrix, 1, GL_FALSE, new_exmatrix.data());
        exmatrix = new_exmatrix;
    }
}

void LineShader::setColor(const std::array<float, 4>& new_color) {
    if (color != new_color) {
        glUniform4fv(u_color, 1, new_color.data());
        color = new_color;
    }
}

void LineShader::setLineWidth(const std::array<float, 2>& new_linewidth) {
    if (linewidth != new_linewidth) {
        glUniform2fv(u_linewidth, 1, new_linewidth.data());
        linewidth = new_linewidth;
    }
}

void LineShader::setRatio(float new_ratio) {
    if (ratio != new_ratio) {
        glUniform1f(u_ratio, new_ratio);
        ratio = new_ratio;
    }
}

void LineShader::setDashArray(const std::array<float, 2>& new_dasharray) {
    if (dasharray != new_dasharray) {
        glUniform2fv(u_dasharray, 1, new_dasharray.data());
        dasharray = new_dasharray;
    }
}

void LineShader::setDebug(float new_debug) {
    if (debug != new_debug) {
        glUniform1f(u_debug, new_debug);
        debug = new_debug;
    }
}
