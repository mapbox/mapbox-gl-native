#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>

#include <cstdio>

using namespace mbgl;

RasterShader::RasterShader()
    : Shader(
         "raster",
         shaders[RASTER_SHADER].vertex,
         shaders[RASTER_SHADER].fragment
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
    u_opacity = glGetUniformLocation(program, "u_opacity");
    u_buffer = glGetUniformLocation(program, "u_buffer");
    u_brightness_low = glGetUniformLocation(program, "u_brightness_low");
    u_brightness_high = glGetUniformLocation(program, "u_brightness_high");
    u_saturation_factor = glGetUniformLocation(program, "u_saturation_factor");
    u_contrast_factor = glGetUniformLocation(program, "u_contrast_factor");
    u_spin_weights = glGetUniformLocation(program, "u_spin_weights");

    // fprintf(stderr, "RasterShader:\n");
    // fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    // fprintf(stderr, "    - u_image: %d\n", u_image);
    // fprintf(stderr, "    - u_opacity: %f\n", u_opacity);
    // fprintf(stderr, "    - u_buffer: %f\n", u_buffer);
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

void RasterShader::setOpacity(float new_opacity) {
    if (opacity != new_opacity) {
        glUniform1f(u_opacity, new_opacity);
        opacity = new_opacity;
    }
}

void RasterShader::setBuffer(float new_buffer) {
    if (buffer != new_buffer) {
        glUniform1f(u_buffer, new_buffer);
        buffer = new_buffer;
    }
}

void RasterShader::setBrightness(float new_brightness_low, float new_brightness_high) {
    if (brightness_low != new_brightness_low) {
        glUniform1f(u_brightness_low, new_brightness_low);
        brightness_low = new_brightness_low;
    }
    if (brightness_high != new_brightness_high) {
        glUniform1f(u_brightness_high, new_brightness_high);
        brightness_high = new_brightness_high;
    }
}

void RasterShader::setSaturation(float new_saturation_factor) {
    if (saturation_factor != new_saturation_factor) {
        if (new_saturation_factor > 0) {
            new_saturation_factor = 1 - 1 / (1.001 - new_saturation_factor);
        } else {
            new_saturation_factor = -new_saturation_factor;
        }
        glUniform1f(u_saturation_factor, new_saturation_factor);
        saturation_factor = new_saturation_factor;
    }
}

void RasterShader::setContrast(float new_contrast_factor) {
    if (contrast_factor != new_contrast_factor) {
        if (new_contrast_factor > 0) {
            new_contrast_factor = 1 / (1 - new_contrast_factor);
        } else {
            new_contrast_factor = 1 + new_contrast_factor;
        }
        glUniform1f(u_contrast_factor, new_contrast_factor);
        contrast_factor = new_contrast_factor;
    }
}

void RasterShader::setSpin(std::array<float, 3> new_spin_weights) {
    if (spin_weights != new_spin_weights) {
        glUniform3fv(u_spin_weights, 1, new_spin_weights.data());
        spin_weights = new_spin_weights;
    }
}