#ifndef MBGL_RENDERER_SHADER_GAUSSIAN
#define MBGL_RENDERER_SHADER_GAUSSIAN

#include <mbgl/shader/shader.hpp>

namespace mbgl {

class GaussianShader : public Shader {
public:
    GaussianShader();

    void bind(char *offset);

    void setImage(int32_t image);
    void setOffset(const std::array<float, 2>& offset);

private:
    int32_t a_pos = -1;

    int32_t image = 0;
    int32_t u_image = -1;

    std::array<float, 2> offset = {{}};
    int32_t u_offset = -1;
};

}

#endif
