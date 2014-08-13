#ifndef MBGL_RENDERER_SHADER_RASTER
#define MBGL_RENDERER_SHADER_RASTER

#include <mbgl/shader/shader.hpp>

namespace mbgl {

class RasterShader : public Shader {
public:
    RasterShader();

    void bind(char *offset);

    void setImage(int32_t image);
    void setOpacity(float opacity);
    void setBuffer(float buffer);
    void setBrightness(float brightness_low, float brightness_high);
    void setSaturation(float saturation_factor);
    void setContrast(float contrast_factor);
    void setSpin(std::array<float, 3> spin_weights);

private:
    int32_t a_pos = -1;

    int32_t image = 0;
    int32_t u_image = -1;

    float opacity = 0;
    int32_t u_opacity = -1;

    float buffer = 0;
    int32_t u_buffer = -1;

    float brightness_low = 0;
    int32_t u_brightness_low = -1;

    float brightness_high = 0;
    int32_t u_brightness_high = -1;

    float saturation_factor = 0;
    int32_t u_saturation_factor = -1;

    float contrast_factor = 1;
    int32_t u_contrast_factor = -1;

    std::array<float, 3> spin_weights = {{}};
    int32_t u_spin_weights = -1;

};

}

#endif
