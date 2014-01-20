#include <llmr/renderer/shader-line.hpp>
#include <llmr/shader/shaders.hpp>
#include <llmr/platform/gl.hpp>

using namespace llmr;

LineShader::LineShader()
    : Shader(
        shaders[LINE_SHADER].vertex,
        shaders[LINE_SHADER].fragment
    ) {
    if (!valid) {
        fprintf(stderr, "invalid line shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");
    attributes.emplace_front(a_pos);

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
}
