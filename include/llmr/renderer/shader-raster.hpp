#ifndef LLMR_RENDERER_SHADER_RASTER
#define LLMR_RENDERER_SHADER_RASTER

#include "shader.hpp"

namespace llmr {

class RasterShader : public Shader {
public:
    RasterShader();

    void bind(char *offset);

    void setImage(int32_t image);

private:
    int32_t a_pos = -1;

    int32_t image = -1;
    int32_t u_image = -1;
};

}

#endif
