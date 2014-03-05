#ifndef LLMR_RENDERER_SHADER_OUTLINE
#define LLMR_RENDERER_SHADER_OUTLINE

#include "shader.hpp"

namespace llmr {

class OutlineShader : public Shader {
public:
    OutlineShader();

    void bind(char *offset);

    void setColor(const std::array<float, 4>& color);
    void setWorld(const std::array<float, 2>& world);

private:
    int32_t a_pos = -1;

    std::array<float, 4> color = {{}};
    int32_t u_color = -1;

    std::array<float, 2> world = {{}};
    int32_t u_world = -1;
};

}

#endif
