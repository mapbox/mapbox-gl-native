#include <mbgl/shader/outline_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>

#include <cstdio>

using namespace mbgl;

OutlineShader::OutlineShader()
    : Shader(
        "outline",
        shaders[OUTLINE_SHADER].vertex,
        shaders[OUTLINE_SHADER].fragment
    ) {
    if (!valid) {
        fprintf(stderr, "invalid outline shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
    u_world = glGetUniformLocation(program, "u_world");

    // fprintf(stderr, "OutlineShader:\n");
    // fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    // fprintf(stderr, "    - u_color: %d\n", u_color);
    // fprintf(stderr, "    - u_world: %d\n", u_world);
}

void OutlineShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset);
}

void OutlineShader::setColor(const std::array<float, 4>& new_color) {
    if (color != new_color) {
        glUniform4fv(u_color, 1, new_color.data());
        color = new_color;
    }
}

void OutlineShader::setWorld(const std::array<float, 2>& new_world) {
    if (world != new_world) {
        glUniform2fv(u_world, 1, new_world.data());
        world = new_world;
    }
}
