#ifndef LLMR_SHADER_SHADER_ICON
#define LLMR_SHADER_SHADER_ICON

#include <llmr/shader/shader.hpp>

namespace llmr {

class IconShader : public Shader {
public:
    IconShader();

    void bind(char *offset);

    void setImage(int32_t image);
    void setColor(const std::array<float, 4>& color);
    void setDimension(const std::array<float, 2>& dimension);
    void setSize(float size);

private:
    int32_t a_pos = -1;
    int32_t a_tex = -1;

    int32_t image = -1;
    int32_t u_image = -1;

    std::array<float, 4> color = {{}};
    int32_t u_color = -1;

    std::array<float, 2> dimension = {{}};
    int32_t u_dimension = -1;

    float size = 0;
    int32_t u_size = -1;
};

}

#endif
