#include <llmr/renderer/shader-raster.hpp>
#include <llmr/shader/shaders.hpp>
#include <llmr/platform/gl.hpp>

#include <cstdio>

using namespace llmr;

RasterShader::RasterShader()
    : Shader(
         shaders[RASTER_SHADER].vertex,
         shaders[RASTER_SHADER].fragment
         ) {
    if (!valid) {
        fprintf(stderr, "invalid raster shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");

    u_matrix = glGetUniformLocation(program, "u_matrix");

    // fprintf(stderr, "RasterShader:\n");
    // fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    // fprintf(stderr, "    - u_image: %d\n", u_image);
}

void RasterShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset);
}

void RasterShader::setImage(int32_t new_image) {
    if (image != new_image) {
        glUniform1i(u_image, new_image);
        image = new_image;
    }
}
