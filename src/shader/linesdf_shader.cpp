#include <mbgl/shader/linesdf_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>

#include <cstdio>

using namespace mbgl;

LineSDFShader::LineSDFShader()
    : Shader(
        shaders[LINESDF_SHADER].vertex,
        shaders[LINESDF_SHADER].fragment
    ) {
    if (!valid) {
        fprintf(stderr, "invalid line shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");
    a_extrude = glGetAttribLocation(program, "a_extrude");
    a_linesofar = glGetAttribLocation(program, "a_linesofar");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_exmatrix = glGetUniformLocation(program, "u_exmatrix");
    u_linewidth = glGetUniformLocation(program, "u_linewidth");
    u_color = glGetUniformLocation(program, "u_color");
    u_blur = glGetUniformLocation(program, "u_blur");
    u_patternscale_a = glGetUniformLocation(program, "u_patternscale_a");
    u_patternscale_b = glGetUniformLocation(program, "u_patternscale_b");
    u_tex_y_a = glGetUniformLocation(program, "u_tex_y_a");
    u_tex_y_b = glGetUniformLocation(program, "u_tex_y_b");
    u_fade = glGetUniformLocation(program, "u_fade");
    u_sdfgamma = glGetUniformLocation(program, "u_sdfgamma");

    // fprintf(stderr, "LineSDFShader:\n");
    // fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    // fprintf(stderr, "    - u_exmatrix: %d\n", u_exmatrix);
    // fprintf(stderr, "    - u_linewidth: %d\n", u_linewidth);
    // fprintf(stderr, "    - u_color: %d\n", u_color);
}

void LineSDFShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 8, offset + 0);

    glEnableVertexAttribArray(a_extrude);
    glVertexAttribPointer(a_extrude, 2, GL_BYTE, false, 8, offset + 4);

    glEnableVertexAttribArray(a_linesofar);
    glVertexAttribPointer(a_linesofar, 1, GL_SHORT, false, 8, offset + 6);
}

void LineSDFShader::setExtrudeMatrix(const std::array<float, 16>& new_exmatrix) {
    if (exmatrix != new_exmatrix) {
        glUniformMatrix4fv(u_exmatrix, 1, GL_FALSE, new_exmatrix.data());
        exmatrix = new_exmatrix;
    }
}

void LineSDFShader::setColor(const std::array<float, 4>& new_color) {
    if (color != new_color) {
        glUniform4fv(u_color, 1, new_color.data());
        color = new_color;
    }
}

void LineSDFShader::setLineWidth(const std::array<float, 2>& new_linewidth) {
    if (linewidth != new_linewidth) {
        glUniform2fv(u_linewidth, 1, new_linewidth.data());
        linewidth = new_linewidth;
    }
}

void LineSDFShader::setPatternScaleB(const std::array<float, 2>& new_patternscale_b) {
    if (patternscale_b != new_patternscale_b) {
        glUniform2fv(u_patternscale_b, 1, new_patternscale_b.data());
        patternscale_b = new_patternscale_b;
    }
}

void LineSDFShader::setPatternScaleA(const std::array<float, 2>& new_patternscale_a) {
    if (patternscale_a != new_patternscale_a) {
        glUniform2fv(u_patternscale_a, 1, new_patternscale_a.data());
        patternscale_a = new_patternscale_a;
    }
}

void LineSDFShader::setBlur(float new_blur) {
    if (blur != new_blur) {
        glUniform1f(u_blur, new_blur);
        blur = new_blur;
    }
}

void LineSDFShader::setTexYA(float new_tex_y_a) {
    if (tex_y_a != new_tex_y_a) {
        glUniform1f(u_tex_y_a, new_tex_y_a);
        tex_y_a = new_tex_y_a;
    }
}

void LineSDFShader::setTexYB(float new_tex_y_b) {
    if (tex_y_b != new_tex_y_b) {
        glUniform1f(u_tex_y_b, new_tex_y_b);
        tex_y_b = new_tex_y_b;
    }
}

void LineSDFShader::setFade(float new_fade) {
    if (fade != new_fade) {
        glUniform1f(u_fade, new_fade);
        fade = new_fade;
    }
}

void LineSDFShader::setGamma(float new_sdfgamma) {
    if (sdfgamma != new_sdfgamma) {
        glUniform1f(u_sdfgamma, new_sdfgamma);
        sdfgamma = new_sdfgamma;
    }
}
