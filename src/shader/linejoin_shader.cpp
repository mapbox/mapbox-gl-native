#include <mbgl/shader/linejoin_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>

#include <cstdio>

using namespace mbgl;

LinejoinShader::LinejoinShader()
    : Shader(
        "linejoin",
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

void LinejoinShader::setColor(const std::array<float, 4>& new_color) {
    if (color != new_color) {
        glUniform4fv(u_color, 1, new_color.data());
        color = new_color;
    }
}

void LinejoinShader::setWorld(const std::array<float, 2>& new_world) {
    if (world != new_world) {
        glUniform2fv(u_world, 1, new_world.data());
        world = new_world;
    }
}

void LinejoinShader::setLineWidth(const std::array<float, 2>& new_linewidth) {
    if (linewidth != new_linewidth) {
        glUniform2fv(u_linewidth, 1, new_linewidth.data());
        linewidth = new_linewidth;
    }
}

void LinejoinShader::setSize(float new_size) {
    if (size != new_size) {
        glUniform1f(u_size, new_size);
        size = new_size;
    }
}
