#ifndef LLMR_RENDERER_SHADER_LINEJOIN
#define LLMR_RENDERER_SHADER_LINEJOIN

#include "shader.hpp"

namespace llmr {

class LinejoinShader : public Shader {
public:
    LinejoinShader();

    void bind(char *offset);

    int32_t a_pos;

    int32_t u_matrix;
    int32_t u_world;
    int32_t u_linewidth;
    int32_t u_color;
};

}

#endif
