#include <mbgl/shader/sdf_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>

#include <cstdio>

using namespace mbgl;

SDFShader::SDFShader()
    : Shader(
        "sdf",
        shaders[SDF_SHADER].vertex,
        shaders[SDF_SHADER].fragment
    ) {
    if (!valid) {
        fprintf(stderr, "invalid sdf shader\n");
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
}

void SDFShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 16, offset + 0);

    glEnableVertexAttribArray(a_offset);
    glVertexAttribPointer(a_offset, 2, GL_SHORT, false, 16, offset + 4);

    glEnableVertexAttribArray(a_tex);
    glVertexAttribPointer(a_tex, 2, GL_UNSIGNED_BYTE, false, 16, offset + 8);

    glEnableVertexAttribArray(a_labelminzoom);
    glVertexAttribPointer(a_labelminzoom, 1, GL_UNSIGNED_BYTE, false, 16,
                          offset + 10);

    glEnableVertexAttribArray(a_minzoom);
    glVertexAttribPointer(a_minzoom, 1, GL_UNSIGNED_BYTE, false, 16,
                          offset + 11);

    glEnableVertexAttribArray(a_maxzoom);
    glVertexAttribPointer(a_maxzoom, 1, GL_UNSIGNED_BYTE, false, 16,
                          offset + 12);

    glEnableVertexAttribArray(a_angle);
    glVertexAttribPointer(a_angle, 1, GL_UNSIGNED_BYTE, false, 16, offset + 13);

    glEnableVertexAttribArray(a_rangeend);
    glVertexAttribPointer(a_rangeend, 1, GL_UNSIGNED_BYTE, false, 16,
                          offset + 14);

    glEnableVertexAttribArray(a_rangestart);
    glVertexAttribPointer(a_rangestart, 1, GL_UNSIGNED_BYTE, false, 16,
                          offset + 15);
}
