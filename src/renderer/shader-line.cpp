#include <llmr/renderer/shader-line.hpp>

using namespace llmr;

LineShader::LineShader(const GLchar *vertSource, const GLchar *fragSource)
    : Shader(vertSource, fragSource) {
    if (!valid) return;

    a_pos = glGetAttribLocation(program, "a_pos");
    attributes.push_back(a_pos);

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
}
