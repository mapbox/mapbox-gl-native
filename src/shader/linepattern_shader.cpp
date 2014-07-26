#include <mbgl/shader/linepattern_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>
#include <iostream>

#include <cstdio>

using namespace mbgl;

LinepatternShader::LinepatternShader()
    : Shader(
        "linepattern",
         shaders[LINEPATTERN_SHADER].vertex,
         shaders[LINEPATTERN_SHADER].fragment
    ) {
    if (!valid) {
        fprintf(stderr, "invalid line pattern shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");
    a_extrude = glGetAttribLocation(program, "a_extrude");
    a_linesofar = glGetAttribLocation(program, "a_linesofar");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_exmatrix = glGetUniformLocation(program, "u_exmatrix");
    u_linewidth = glGetUniformLocation(program, "u_linewidth");
    u_ratio = glGetUniformLocation(program, "u_ratio");
    u_pattern_size = glGetUniformLocation(program, "u_pattern_size");

    u_pattern_tl = glGetUniformLocation(program, "u_pattern_tl");
    u_pattern_br = glGetUniformLocation(program, "u_pattern_br");
    u_offset = glGetUniformLocation(program, "u_offset");
    u_gamma = glGetUniformLocation(program, "u_gamma");
    u_fade = glGetUniformLocation(program, "u_fade");

    //fprintf(stderr, "LinepatternShader:\n");
    //fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    //fprintf(stderr, "    - u_exmatrix: %d\n", u_exmatrix);
    //fprintf(stderr, "    - u_linewidth: %d\n", u_linewidth);
    //fprintf(stderr, "    - u_ratio: %d\n", u_ratio);
    //fprintf(stderr, "    - u_pattern_size: %d\n", u_pattern_size);
    //fprintf(stderr, "    - u_pattern_tl: %d\n", u_pattern_tl);
    //fprintf(stderr, "    - u_pattern_br: %d\n", u_pattern_br);
    //fprintf(stderr, "    - u_offset: %d\n", u_offset);
    //fprintf(stderr, "    - u_gamma: %d\n", u_gamma);
}

void LinepatternShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 8, offset + 0);

    glEnableVertexAttribArray(a_extrude);
    glVertexAttribPointer(a_extrude, 2, GL_BYTE, false, 8, offset + 4);

    glEnableVertexAttribArray(a_linesofar);
    glVertexAttribPointer(a_linesofar, 1, GL_SHORT, false, 8, offset + 6);
}

void LinepatternShader::setExtrudeMatrix(const std::array<float, 16>& new_exmatrix) {
    if (exmatrix != new_exmatrix) {
        glUniformMatrix4fv(u_exmatrix, 1, GL_FALSE, new_exmatrix.data());
        exmatrix = new_exmatrix;
    }
}

void LinepatternShader::setPatternSize(const std::array<float, 2>& new_pattern_size) {
    if (pattern_size != new_pattern_size) {
        glUniform2fv(u_pattern_size, 1, new_pattern_size.data());
        pattern_size = new_pattern_size;
    }
}

void LinepatternShader::setLineWidth(const std::array<float, 2>& new_linewidth) {
    if (linewidth != new_linewidth) {
        glUniform2fv(u_linewidth, 1, new_linewidth.data());
        linewidth = new_linewidth;
    }
}

void LinepatternShader::setRatio(float new_ratio) {
    if (ratio != new_ratio) {
        glUniform1f(u_ratio, new_ratio);
        ratio = new_ratio;
    }
}

void LinepatternShader::setPatternTopLeft(const std::array<float, 2>& new_pattern_tl) {
    if (pattern_tl != new_pattern_tl) {
        glUniform2fv(u_pattern_tl, 1, new_pattern_tl.data());
        pattern_tl = new_pattern_tl;
    }
}

void LinepatternShader::setPatternBottomRight(const std::array<float, 2>& new_pattern_br) {
    if (pattern_br != new_pattern_br) {
        glUniform2fv(u_pattern_br, 1, new_pattern_br.data());
        pattern_br = new_pattern_br;
    }
}

void LinepatternShader::setOffset(const std::array<float, 2>& new_offset) {
    if (offset != new_offset) {
        glUniform2fv(u_offset, 1, new_offset.data());
        offset = new_offset;
    }
}

void LinepatternShader::setGamma(float new_gamma) {
    if (gamma != new_gamma) {
        glUniform1f(u_gamma, new_gamma);
        gamma = new_gamma;
    }
}

void LinepatternShader::setFade(float new_fade) {
    if (fade != new_fade) {
        glUniform1f(u_fade, new_fade);
        fade = new_fade;
    }
};
