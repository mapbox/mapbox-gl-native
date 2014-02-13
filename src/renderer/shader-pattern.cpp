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

    // fprintf(stderr, "PatternShader:\n");
    // fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    // fprintf(stderr, "    - u_color: %d\n", u_color);
    // fprintf(stderr, "    - u_offset: %d\n", u_offset);
    // fprintf(stderr, "    - u_pattern_size: %d\n", u_pattern_size);
    // fprintf(stderr, "    - u_pattern_tl: %d\n", u_pattern_tl);
    // fprintf(stderr, "    - u_pattern_br: %d\n", u_pattern_br);
    // fprintf(stderr, "    - u_mix: %d\n", u_mix);
}

void PatternShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset);
}
