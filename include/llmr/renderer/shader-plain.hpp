#ifndef LLMR_RENDERER_SHADER_PLAIN
#define LLMR_RENDERER_SHADER_PLAIN

#include "shader.hpp"

namespace llmr {

class PlainShader : public Shader {
public:
    PlainShader();

    void bind(char *offset);

    int32_t a_pos;
    int32_t u_matrix;
    int32_t u_color;
};

}

#endif
