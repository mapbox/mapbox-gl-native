#ifndef LLMR_RENDERER_SHADER_PATTERN
#define LLMR_RENDERER_SHADER_PATTERN

#include "shader.hpp"

namespace llmr {

class PatternShader : public Shader {
public:
    PatternShader();

    void bind(char *offset);

    int32_t a_pos;
    int32_t u_matrix;
    int32_t u_color;
    int32_t u_offset;
    int32_t u_pattern_size;
    int32_t u_pattern_tl;
    int32_t u_pattern_br;
    int32_t u_mix;

};

}

#endif
