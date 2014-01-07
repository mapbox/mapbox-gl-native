#ifndef LLMR_RENDERER_SHADER_FILL
#define LLMR_RENDERER_SHADER_FILL

#include "shader.hpp"

namespace llmr {

class FillShader : public Shader {
public:
    FillShader();

    int32_t a_pos;
    int32_t u_matrix;
    int32_t u_color;
};

}

#endif
