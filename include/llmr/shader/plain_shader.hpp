#ifndef LLMR_SHADER_SHADER_PLAIN
#define LLMR_SHADER_SHADER_PLAIN

#include <llmr/shader/shader.hpp>

namespace llmr {

class PlainShader : public Shader {
public:
    PlainShader();

    void bind(char *offset);

    void setColor(float r, float g, float b, float a);
    void setColor(const std::array<float, 4>& color);
    void setOpacity(float opacity);

private:
    int32_t a_pos = -1;

    std::array<float, 4> color = {{}};
    int32_t u_color = -1;
    float opacity = 0.0f;
    float u_opacity = 0.0f;
};

}

#endif
