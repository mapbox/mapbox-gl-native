#include <llmr/shader/icon_shader.hpp>
#include <llmr/shader/shaders.hpp>
#include <llmr/platform/gl.hpp>

#include <cstdio>

using namespace llmr;

IconShader::IconShader()
    : Shader(
         shaders[ICON_SHADER].vertex,
         shaders[ICON_SHADER].fragment
         ) {
    if (!valid) {
        fprintf(stderr, "invalid icon shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");
    a_tex = glGetAttribLocation(program, "a_tex");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
    u_size = glGetUniformLocation(program, "u_size");
    u_ratio = glGetUniformLocation(program, "u_ratio");
    u_dimension = glGetUniformLocation(program, "u_dimension");

    // fprintf(stderr, "IconShader:\n");
    // fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    // fprintf(stderr, "    - u_color: %d\n", u_color);
    // fprintf(stderr, "    - u_size: %d\n", u_size);
    // fprintf(stderr, "    - u_ratio: %d\n", u_ratio);
    // fprintf(stderr, "    - u_dimension: %d\n", u_dimension);
    // fprintf(stderr, "    - u_image: %d\n", u_image);
}

void IconShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 8, offset);

    glEnableVertexAttribArray(a_tex);
    glVertexAttribPointer(a_tex, 2, GL_UNSIGNED_SHORT, false, 8, offset + 4);
}

void IconShader::setImage(int32_t new_image) {
    if (image != new_image) {
        glUniform1i(u_image, new_image);
        image = new_image;
    }
}

void IconShader::setColor(const std::array<float, 4>& new_color) {
    if (color != new_color) {
        glUniform4fv(u_color, 1, new_color.data());
        color = new_color;
    }
}

void IconShader::setSize(float new_size) {
    if (size != new_size) {
        glUniform1f(u_size, new_size);
        size = new_size;
    }
}

void IconShader::setRatio(float new_ratio) {
    if (ratio != new_ratio) {
        glUniform1f(u_ratio, new_ratio);
        ratio = new_ratio;
    }
}

void IconShader::setDimension(const std::array<float, 2>& new_dimension) {
    if (dimension != new_dimension) {
        glUniform2fv(u_dimension, 1, new_dimension.data());
        dimension = new_dimension;
    }
}
