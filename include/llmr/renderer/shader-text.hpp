#ifndef LLMR_RENDERER_SHADER_TEXT
#define LLMR_RENDERER_SHADER_TEXT

#include "shader.hpp"

namespace llmr {

class TextShader : public Shader {
public:
    TextShader();

    void bind(char *offset);

    void setColor(float r, float g, float b, float a);
    void setColor(const std::array<float, 4>& color);

private:
    int32_t a_pos = -1;

    std::array<float, 4> color = {{}};
    int32_t u_color = -1;
};

}

#endif
