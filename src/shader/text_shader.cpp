#include <mbgl/shader/text_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>

#include <cstdio>

using namespace mbgl;

TextShader::TextShader()
    : Shader(
        "text",
        shaders[TEXT_SHADER].vertex,
        shaders[TEXT_SHADER].fragment
    ) {
    if (!valid) {
        fprintf(stderr, "invalid text shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");
    a_offset = glGetAttribLocation(program, "a_offset");
    a_data1 = glGetAttribLocation(program, "a_data1");
    a_data2 = glGetAttribLocation(program, "a_data2");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
    u_buffer = glGetUniformLocation(program, "u_buffer");
    u_gamma = glGetUniformLocation(program, "u_gamma");
    u_exmatrix = glGetUniformLocation(program, "u_exmatrix");
    u_angle = glGetUniformLocation(program, "u_angle");
    u_zoom = glGetUniformLocation(program, "u_zoom");
    u_flip = glGetUniformLocation(program, "u_flip");
    u_fadedist = glGetUniformLocation(program, "u_fadedist");
    u_minfadezoom = glGetUniformLocation(program, "u_minfadezoom");
    u_maxfadezoom = glGetUniformLocation(program, "u_maxfadezoom");
    u_fadezoom = glGetUniformLocation(program, "u_fadezoom");
    u_texsize = glGetUniformLocation(program, "u_texsize");

    // fprintf(stderr, "TextShader:\n");
    // fprintf(stderr, "    - a_pos: %d\n", a_pos);
    // fprintf(stderr, "    - a_offset: %d\n", a_offset);
    // fprintf(stderr, "    - a_data1: %d\n", a_tex);
    // fprintf(stderr, "    - a_data2: %d\n", a_tex);
    // fprintf(stderr, "    - u_color: %d\n", u_color);
    // fprintf(stderr, "    - u_buffer: %d\n", u_buffer);
    // fprintf(stderr, "    - u_gamma: %d\n", u_gamma);
    // fprintf(stderr, "    - u_exmatrix: %d\n", u_exmatrix);
    // fprintf(stderr, "    - u_angle: %d\n", u_angle);
    // fprintf(stderr, "    - u_zoom: %d\n", u_zoom);
    // fprintf(stderr, "    - u_flip: %d\n", u_flip);
    // fprintf(stderr, "    - u_fadedist: %d\n", u_fadedist);
    // fprintf(stderr, "    - u_minfadezoom: %d\n", u_minfadezoom);
    // fprintf(stderr, "    - u_maxfadezoom: %d\n", u_maxfadezoom);
    // fprintf(stderr, "    - u_fadezoom: %d\n", u_fadezoom);
    // fprintf(stderr, "    - u_texsize: %d\n", u_texsize);
    // fprintf(stderr, "    - u_opacity: %d\n", u_opacity);
}

void TextShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 16, offset + 0);

    glEnableVertexAttribArray(a_offset);
    glVertexAttribPointer(a_offset, 2, GL_SHORT, false, 16, offset + 4);

    glEnableVertexAttribArray(a_data1);
    glVertexAttribPointer(a_data1, 4, GL_UNSIGNED_BYTE, false, 16, offset + 8);

    glEnableVertexAttribArray(a_data2);
    glVertexAttribPointer(a_data2, 4, GL_UNSIGNED_BYTE, false, 16, offset + 12);
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

void TextShader::setBuffer(float new_buffer) {
    if (buffer != new_buffer) {
        glUniform1f(u_buffer, new_buffer);
        buffer = new_buffer;
    }
}

void TextShader::setGamma(float new_gamma) {
    if (gamma != new_gamma) {
        glUniform1f(u_gamma, new_gamma);
        gamma = new_gamma;
    }
}

void TextShader::setExtrudeMatrix(const std::array<float, 16> &new_exmatrix) {
    if (exmatrix != new_exmatrix) {
        glUniformMatrix4fv(u_exmatrix, 1, GL_FALSE, new_exmatrix.data());
        exmatrix = new_exmatrix;
    }
}

void TextShader::setAngle(float new_angle) {
    if (angle != new_angle) {
        glUniform1f(u_angle, new_angle);
        angle = new_angle;
    }
}

void TextShader::setZoom(float new_zoom) {
    if (zoom != new_zoom) {
        glUniform1f(u_zoom, new_zoom);
        zoom = new_zoom;
    }
}

void TextShader::setFlip(float new_flip) {
    if (flip != new_flip) {
        glUniform1f(u_flip, new_flip);
        flip = new_flip;
    }
}

void TextShader::setFadeDist(float new_fadedist) {
    if (fadedist != new_fadedist) {
        glUniform1f(u_fadedist, new_fadedist);
        fadedist = new_fadedist;
    }
}

void TextShader::setMinFadeZoom(float new_minfadezoom) {
    if (minfadezoom != new_minfadezoom) {
        glUniform1f(u_minfadezoom, new_minfadezoom);
        minfadezoom = new_minfadezoom;
    }
}

void TextShader::setMaxFadeZoom(float new_maxfadezoom) {
    if (maxfadezoom != new_maxfadezoom) {
        glUniform1f(u_maxfadezoom, new_maxfadezoom);
        maxfadezoom = new_maxfadezoom;
    }
}

void TextShader::setFadeZoom(float new_fadezoom) {
    if (fadezoom != new_fadezoom) {
        glUniform1f(u_fadezoom, new_fadezoom);
        fadezoom = new_fadezoom;
    }
}

void TextShader::setTextureSize(const std::array<float, 2> &new_texsize) {
    if (texsize != new_texsize) {
        glUniform2fv(u_texsize, 1, new_texsize.data());
        texsize = new_texsize;
    }
}
