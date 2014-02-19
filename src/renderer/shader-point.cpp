#include <llmr/renderer/shader-point.hpp>
#include <llmr/shader/shaders.hpp>
#include <llmr/platform/gl.hpp>

#include <cstdio>

using namespace llmr;

PointShader::PointShader()
    : Shader(
         shaders[POINT_SHADER].vertex,
         shaders[POINT_SHADER].fragment
         ) {
    if (!valid) {
        fprintf(stderr, "invalid point shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
    u_size = glGetUniformLocation(program, "u_size");
    u_point_tl = glGetUniformLocation(program, "u_tl");
    u_point_br = glGetUniformLocation(program, "u_br");

    // fprintf(stderr, "PointShader:\n");
    // fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    // fprintf(stderr, "    - u_color: %d\n", u_color);
    // fprintf(stderr, "    - u_size: %d\n", u_size);
    // fprintf(stderr, "    - u_point_tl: %d\n", u_point_tl);
    // fprintf(stderr, "    - u_point_br: %d\n", u_point_br);
}

void PointShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset);
}

void PointShader::setColor(const std::array<float, 4>& new_color) {
    if (color != new_color) {
        glUniform4fv(u_color, 1, new_color.data());
        color = new_color;
    }
}

void PointShader::setSize(float new_size) {
    if (size != new_size) {
        glUniform1f(u_size, new_size);
        size = new_size;
    }
}

void PointShader::setPointTopLeft(const std::array<float, 2>& new_point_tl) {
    if (point_tl != new_point_tl) {
        glUniform2fv(u_point_tl, 1, new_point_tl.data());
        point_tl = new_point_tl;
    }
}

void PointShader::setPointBottomRight(const std::array<float, 2>& new_point_br) {
    if (point_br != new_point_br) {
        glUniform2fv(u_point_br, 1, new_point_br.data());
        point_br = new_point_br;
    }
}
