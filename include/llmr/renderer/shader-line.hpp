#ifndef LLMR_RENDERER_SHADER_LINE
#define LLMR_RENDERER_SHADER_LINE

#include "shader.hpp"

namespace llmr {

class LineShader : public Shader {
public:
    LineShader(const GLchar *vertex, const GLchar *fragment);

    GLint a_pos;
    GLint u_matrix;
    GLint u_color;
};

}

#endif
