#ifndef LLMR_RENDERER_SHADER_PLAIN
#define LLMR_RENDERER_SHADER_PLAIN

#include "shader.hpp"

namespace llmr {

class PlainShader : public Shader {
public:
    PlainShader();

    void bind(char *offset);

    void setColor(float r, float g, float b, float a);
    void setColor(const std::array<float, 4>& color);

private:
    int32_t a_pos;

    std::array<float, 4> color;
    int32_t u_color;
};

}

#endif
