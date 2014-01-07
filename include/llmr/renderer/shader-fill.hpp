#ifndef LLMR_RENDERER_SHADER_FILL
#define LLMR_RENDERER_SHADER_FILL

#include "shader.hpp"

namespace llmr {

class FillShader : public Shader {
public:
    FillShader(const GLchar *vertex, const GLchar *fragment);

    GLint a_pos;
    GLint u_matrix;
    GLint u_color;
};

}

#endif
