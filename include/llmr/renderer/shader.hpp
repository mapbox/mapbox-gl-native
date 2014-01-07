#ifndef LLMR_RENDERER_SHADER
#define LLMR_RENDERER_SHADER

#include "../platform/gl.hpp"
#include <vector>

namespace llmr {

class Shader {
public:
    Shader(const GLchar *vertex, const GLchar *fragment);
    ~Shader();
    bool valid;
    GLuint program;
    std::vector<GLuint> attributes;

private:
    bool compileShader(GLuint *shader, GLenum type, const GLchar *source);
};

}

#endif
