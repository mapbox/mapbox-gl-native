#include <llmr/shader/point_shader.hpp>
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
    u_pos = glGetUniformLocation(program, "u_pos");
    u_dimension = glGetUniformLocation(program, "u_dimension");

    // fprintf(stderr, "PointShader:\n");
    // fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    // fprintf(stderr, "    - u_color: %d\n", u_color);
    // fprintf(stderr, "    - u_size: %d\n", u_size);
    // fprintf(stderr, "    - u_pos: %d\n", u_pos);
    // fprintf(stderr, "    - u_dimension: %d\n", u_dimension);
    // fprintf(stderr, "    - u_image: %d\n", u_image);
}

void PointShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset);
}

void PointShader::setImage(int32_t new_image) {
    if (image != new_image) {
        glUniform1i(u_image, new_image);
        image = new_image;
    }
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

void PointShader::setPosition(const std::array<float, 2>& new_pos) {
    if (pos != new_pos) {
        glUniform2fv(u_pos, 1, new_pos.data());
        pos = new_pos;
    }
}

void PointShader::setDimension(const std::array<float, 2>& new_dimension) {
    if (dimension != new_dimension) {
        glUniform2fv(u_dimension, 1, new_dimension.data());
        dimension = new_dimension;
    }
}
