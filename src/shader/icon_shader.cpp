#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>

#include <cstdio>

using namespace mbgl;

IconShader::IconShader()
    : Shader(
         "icon",
         shaders[ICON_SHADER].vertex,
         shaders[ICON_SHADER].fragment
         ) {
    if (!valid) {
        fprintf(stderr, "invalid icon shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");
    a_offset = glGetAttribLocation(program, "a_offset");
    a_tex = glGetAttribLocation(program, "a_tex");
    a_angle = glGetAttribLocation(program, "a_angle");
    a_minzoom = glGetAttribLocation(program, "a_minzoom");
    a_maxzoom = glGetAttribLocation(program, "a_maxzoom");
    a_rangeend = glGetAttribLocation(program, "a_rangeend");
    a_rangestart = glGetAttribLocation(program, "a_rangestart");
    a_labelminzoom = glGetAttribLocation(program, "a_labelminzoom");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_exmatrix = glGetUniformLocation(program, "u_exmatrix");
    u_angle = glGetUniformLocation(program, "u_angle");
    u_zoom = glGetUniformLocation(program, "u_zoom");
    u_flip = glGetUniformLocation(program, "u_flip");
    u_fadedist = glGetUniformLocation(program, "u_fadedist");
    u_minfadezoom = glGetUniformLocation(program, "u_minfadezoom");
    u_maxfadezoom = glGetUniformLocation(program, "u_maxfadezoom");
    u_fadezoom = glGetUniformLocation(program, "u_fadezoom");
    u_opacity = glGetUniformLocation(program, "u_opacity");
    u_texsize = glGetUniformLocation(program, "u_texsize");

    // fprintf(stderr, "IconShader:\n");
    // fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    // fprintf(stderr, "    - u_exmatrix: %d\n", u_exmatrix);
    // fprintf(stderr, "    - u_angle: %d\n", u_angle);
    // fprintf(stderr, "    - u_zoom: %d\n", u_zoom);
    // fprintf(stderr, "    - u_flip: %d\n", u_flip);
    // fprintf(stderr, "    - u_fadedist: %d\n", u_fadedist);
    // fprintf(stderr, "    - u_minfadezoom: %d\n", u_minfadezoom);
    // fprintf(stderr, "    - u_maxfadezoom: %d\n", u_maxfadezoom);
    // fprintf(stderr, "    - u_fadezoom: %d\n", u_fadezoom);
    // fprintf(stderr, "    - u_opacity: %d\n", u_opacity);
}

void IconShader::bind(char *offset) {
    const int stride = 20;

    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, stride, offset + 0);

    glEnableVertexAttribArray(a_offset);
    glVertexAttribPointer(a_offset, 2, GL_SHORT, false, stride, offset + 4);

    glEnableVertexAttribArray(a_labelminzoom);
    glVertexAttribPointer(a_labelminzoom, 1, GL_UNSIGNED_BYTE, false, stride, offset + 8);

    glEnableVertexAttribArray(a_minzoom);
    glVertexAttribPointer(a_minzoom, 1, GL_UNSIGNED_BYTE, false, stride, offset + 9);

    glEnableVertexAttribArray(a_maxzoom);
    glVertexAttribPointer(a_maxzoom, 1, GL_UNSIGNED_BYTE, false, stride, offset + 10);

    glEnableVertexAttribArray(a_angle);
    glVertexAttribPointer(a_angle, 1, GL_UNSIGNED_BYTE, false, stride, offset + 11);

    glEnableVertexAttribArray(a_rangeend);
    glVertexAttribPointer(a_rangeend, 1, GL_UNSIGNED_BYTE, false, stride, offset + 12);

    glEnableVertexAttribArray(a_rangestart);
    glVertexAttribPointer(a_rangestart, 1, GL_UNSIGNED_BYTE, false, stride, offset + 13);

    glEnableVertexAttribArray(a_tex);
    glVertexAttribPointer(a_tex, 2, GL_SHORT, false, stride, offset + 16);
}

void IconShader::setExtrudeMatrix(const std::array<float, 16>& new_exmatrix) {
    if (exmatrix != new_exmatrix) {
        glUniformMatrix4fv(u_exmatrix, 1, GL_FALSE, new_exmatrix.data());
        exmatrix = new_exmatrix;
    }
}

void IconShader::setAngle(float new_angle) {
    if (angle != new_angle) {
        glUniform1f(u_angle, new_angle);
        angle = new_angle;
    }
}

void IconShader::setZoom(float new_zoom) {
    if (zoom != new_zoom) {
        glUniform1f(u_zoom, new_zoom);
        zoom = new_zoom;
    }
}

void IconShader::setFlip(float new_flip) {
    if (flip != new_flip) {
        glUniform1f(u_flip, new_flip);
        flip = new_flip;
    }
}

void IconShader::setFadeDist(float new_fadedist) {
    if (fadedist != new_fadedist) {
        glUniform1f(u_fadedist, new_fadedist);
        fadedist = new_fadedist;
    }
}

void IconShader::setMinFadeZoom(float new_minfadezoom) {
    if (minfadezoom != new_minfadezoom) {
        glUniform1f(u_minfadezoom, new_minfadezoom);
        minfadezoom = new_minfadezoom;
    }
}

void IconShader::setMaxFadeZoom(float new_maxfadezoom) {
    if (maxfadezoom != new_maxfadezoom) {
        glUniform1f(u_maxfadezoom, new_maxfadezoom);
        maxfadezoom = new_maxfadezoom;
    }
}

void IconShader::setFadeZoom(float new_fadezoom) {
    if (fadezoom != new_fadezoom) {
        glUniform1f(u_fadezoom, new_fadezoom);
        fadezoom = new_fadezoom;
    }
}

void IconShader::setOpacity(float new_opacity) {
    if (opacity != new_opacity) {
        glUniform1f(u_opacity, new_opacity);
        opacity = new_opacity;
    }
}

void IconShader::setTextureSize(const std::array<float, 2> &new_texsize) {
    if (texsize != new_texsize) {
        glUniform2fv(u_texsize, 1, new_texsize.data());
        texsize = new_texsize;
    }
}

