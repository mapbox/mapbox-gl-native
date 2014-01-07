#ifndef LLMR_RENDERER_SHADER_LINE
#define LLMR_RENDERER_SHADER_LINE

#include "shader.hpp"

namespace llmr {

class LineShader : public Shader {
public:
    LineShader();

    int32_t a_pos;
    int32_t u_matrix;
    int32_t u_color;
};

}

#endif
