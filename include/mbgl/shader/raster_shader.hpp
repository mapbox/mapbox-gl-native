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

private:
    int32_t a_pos = -1;

    int32_t image = 0;
    int32_t u_image = -1;

    float opacity = 0;
    int32_t u_opacity = -1;

    float buffer = 0;
    int32_t u_buffer = -1;
};

}

#endif
