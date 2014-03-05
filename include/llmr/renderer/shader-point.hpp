#ifndef LLMR_RENDERER_SHADER_POINT
#define LLMR_RENDERER_SHADER_POINT

#include "shader.hpp"

namespace llmr {

class PointShader : public Shader {
public:
    PointShader();

    void bind(char *offset);

    void setImage(int32_t image);
    void setColor(const std::array<float, 4>& color);
    void setPointTopLeft(const std::array<float, 2>& point_tl);
    void setPointBottomRight(const std::array<float, 2>& point_br);
    void setSize(float size);

private:
    int32_t a_pos = -1;

    int32_t image = -1;
    int32_t u_image = -1;

    std::array<float, 4> color = {{}};
    int32_t u_color = -1;

    std::array<float, 2> point_tl = {{}};
    int32_t u_point_tl = -1;

    std::array<float, 2> point_br = {{}};
    int32_t u_point_br = -1;

    float size = 0;
    int32_t u_size = -1;
};

}

#endif
