#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>

#include <cstdio>

using namespace mbgl;

PatternShader::PatternShader()
    : Shader(
        "pattern",
        shaders[PATTERN_SHADER].vertex,
        shaders[PATTERN_SHADER].fragment
    ) {
    if (!valid) {
        fprintf(stderr, "invalid pattern shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_pattern_tl = glGetUniformLocation(program, "u_pattern_tl");
    u_pattern_br = glGetUniformLocation(program, "u_pattern_br");
    u_opacity = glGetUniformLocation(program, "u_opacity");
    u_image = glGetUniformLocation(program, "u_image");
    u_mix = glGetUniformLocation(program, "u_mix");
    u_patternmatrix = glGetUniformLocation(program, "u_patternmatrix");

    // fprintf(stderr, "PatternShader:\n");
    // fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    // fprintf(stderr, "    - u_pattern_tl: %d\n", u_pattern_tl);
    // fprintf(stderr, "    - u_pattern_br: %d\n", u_pattern_br);
    // fprintf(stderr, "    - u_opacity: %d\n", u_opacity);
    // fprintf(stderr, "    - u_image: %d\n", u_image);
    // fprintf(stderr, "    - u_mix: %d\n", u_mix);
    // fprintf(stderr, "    - u_patternmatrix: %d\n", u_patternmatrix);
}

void PatternShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset);
}

void PatternShader::setPatternTopLeft(const std::array<float, 2>& new_pattern_tl) {
    if (pattern_tl != new_pattern_tl) {
        glUniform2fv(u_pattern_tl, 1, new_pattern_tl.data());
        pattern_tl = new_pattern_tl;
    }
}

void PatternShader::setPatternBottomRight(const std::array<float, 2>& new_pattern_br) {
    if (pattern_br != new_pattern_br) {
        glUniform2fv(u_pattern_br, 1, new_pattern_br.data());
        pattern_br = new_pattern_br;
    }
}

void PatternShader::setOpacity(float new_opacity) {
    if (opacity != new_opacity) {
        glUniform1f(u_opacity, new_opacity);
        opacity = new_opacity;
    }
}

void PatternShader::setImage(int new_image) {
    if (image != new_image) {
        glUniform1i(u_image, new_image);
        image = new_image;
    }
}

void PatternShader::setMix(float new_mix) {
    if (mix != new_mix) {
        glUniform1f(u_mix, new_mix);
        mix = new_mix;
    }
}

void PatternShader::setPatternMatrix(const std::array<float, 9>& new_patternmatrix) {
    if (patternmatrix != new_patternmatrix) {
        glUniformMatrix3fv(u_patternmatrix, 1, GL_FALSE, new_patternmatrix.data());
        patternmatrix = new_patternmatrix;
    }
}

