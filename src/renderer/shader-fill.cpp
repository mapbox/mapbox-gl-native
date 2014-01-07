#include <llmr/renderer/shader-fill.hpp>
#include <llmr/shader/shaders.h>
#include <llmr/platform/gl.hpp>

using namespace llmr;

FillShader::FillShader()
    : Shader(
        llmr_shaders[FILL_SHADER].vertex,
        llmr_shaders[FILL_SHADER].fragment
    ) {
    if (!valid) return;

    a_pos = glGetAttribLocation(program, "a_pos");
    attributes.push_back(a_pos);

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
}
