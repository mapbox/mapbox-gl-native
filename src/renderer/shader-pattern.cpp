#include <llmr/renderer/shader-pattern.hpp>
#include <llmr/shader/shaders.hpp>
#include <llmr/platform/gl.hpp>

#include <cstdio>

using namespace llmr;

PatternShader::PatternShader()
    : Shader(
        shaders[PATTERN_SHADER].vertex,
        shaders[PATTERN_SHADER].fragment
    ) {
    if (!valid) {
        fprintf(stderr, "invalid pattern shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
    u_offset = glGetUniformLocation(program, "u_offset");
    u_pattern_size = glGetUniformLocation(program, "u_pattern_size");
    u_pattern_tl = glGetUniformLocation(program, "u_pattern_tl");
    u_pattern_br = glGetUniformLocation(program, "u_pattern_br");
    u_mix = glGetUniformLocation(program, "u_mix");
}

void PatternShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset);
}
