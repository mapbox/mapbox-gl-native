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

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_image = glGetUniformLocation(program, "u_image");
    u_offset = glGetUniformLocation(program, "u_offset");

    // fprintf(stderr, "GaussianShader:\n");
    // fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    // fprintf(stderr, "    - u_image: %d\n", u_image);
    // fprintf(stderr, "    - u_gaussian: %f\n", u_gaussian);
}

void GaussianShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset);
}

void GaussianShader::setImage(int32_t new_image) {
    if (image != new_image) {
        glUniform1i(u_image, new_image);
        image = new_image;
    }
}

void GaussianShader::setOffset(const std::array<float, 2>& new_offset) {
    if (offset != new_offset) {
        glUniform2fv(u_offset, 1, new_offset.data());
        offset = new_offset;
    }
}
