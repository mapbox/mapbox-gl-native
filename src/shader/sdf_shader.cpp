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
    a_data1 = glGetAttribLocation(program, "a_data1");
    a_data2 = glGetAttribLocation(program, "a_data2");
}

void SDFShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 16, offset + 0);

    glEnableVertexAttribArray(a_offset);
    glVertexAttribPointer(a_offset, 2, GL_SHORT, false, 16, offset + 4);

    glEnableVertexAttribArray(a_data1);
    glVertexAttribPointer(a_data1, 4, GL_UNSIGNED_BYTE, false, 16, offset + 8);

    glEnableVertexAttribArray(a_data2);
    glVertexAttribPointer(a_data2, 4, GL_UNSIGNED_BYTE, false, 16, offset + 12);
}
