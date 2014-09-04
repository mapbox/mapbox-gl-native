#include <mbgl/shader/gaussian_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>

#include <cstdio>

using namespace mbgl;

GaussianShader::GaussianShader()
    : Shader(
         "gaussian",
         shaders[GAUSSIAN_SHADER].vertex,
         shaders[GAUSSIAN_SHADER].fragment
         ) {
    if (!valid) {
#if defined(DEBUG)
        fprintf(stderr, "invalid raster shader\n");
#endif
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");
}

void GaussianShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset);
}
