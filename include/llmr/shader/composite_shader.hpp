#ifndef LLMR_SHADER_COMPOSITE_SHADER
#define LLMR_SHADER_COMPOSITE_SHADER

#include <llmr/shader/shader.hpp>

namespace llmr {

class CompositeShader : public Shader {
public:
    CompositeShader();

    void bind(char *offset);

    void setImage(int32_t image);
    void setOpacity(float opacity);

private:
    int32_t a_pos = -1;

    int32_t image = 0;
    int32_t u_image = -1;

    float opacity = 0;
    int32_t u_opacity = -1;
};

}

#endif
