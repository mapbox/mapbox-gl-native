#ifndef LLMR_RENDERER_SHADER_LINE
#define LLMR_RENDERER_SHADER_LINE

#include "shader.hpp"

namespace llmr {

class LineShader : public Shader {
public:
    LineShader();

    void bind(char *offset);

    int32_t a_pos;
    int32_t a_extrude;
    // int32_t a_linesofar;

    int32_t u_exmatrix;
    int32_t u_linewidth;
    int32_t u_color;
    int32_t u_ratio;
    int32_t u_dasharray;
    int32_t u_debug;
};

}

#endif
