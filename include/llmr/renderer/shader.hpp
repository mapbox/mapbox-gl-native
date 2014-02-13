#ifndef LLMR_RENDERER_SHADER
#define LLMR_RENDERER_SHADER

#include <cstdint>
#include <llmr/util/noncopyable.hpp>
#include <llmr/util/mat4.hpp>

namespace llmr {

class Shader : private util::noncopyable {
public:
    Shader(const char *vertex, const char *fragment);
    ~Shader();
    bool valid;
    uint32_t program;

    void setMatrix(const mat4& matrix);

private:
    bool compileShader(uint32_t *shader, uint32_t type, const char *source);

protected:
    int32_t u_matrix;
    mat4 matrix;
};

}

#endif
