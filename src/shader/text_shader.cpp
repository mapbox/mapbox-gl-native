#include <llmr/shader/text_shader.hpp>
#include <llmr/shader/shaders.hpp>
#include <llmr/platform/gl.hpp>

#include <cstdio>

using namespace llmr;

TextShader::TextShader()
    : Shader(
        shaders[TEXT_SHADER].vertex,
        shaders[TEXT_SHADER].fragment
    ) {
    if (!valid) {
        fprintf(stderr, "invalid text shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");

    // fprintf(stderr, "TextShader:\n");
    // fprintf(stderr, "    - a_pos: %d\n", a_pos);
    // fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    // fprintf(stderr, "    - u_color: %d\n", u_color);
}

void TextShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset);
}

void TextShader::setColor(const std::array<float, 4>& new_color) {
    if (color != new_color) {
        glUniform4fv(u_color, 1, new_color.data());
        color = new_color;
    }
}

void TextShader::setColor(float r, float g, float b, float a) {
    setColor({{ r, g, b, a }});
}
