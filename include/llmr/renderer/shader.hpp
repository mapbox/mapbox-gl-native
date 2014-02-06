#ifndef LLMR_RENDERER_SHADER
#define LLMR_RENDERER_SHADER

#include <cstdint>
#include <llmr/util/noncopyable.hpp>

namespace llmr {

class Shader : private util::noncopyable {
public:
    Shader(const char *vertex, const char *fragment);
    ~Shader();
    bool valid;
    uint32_t program;

private:
    bool compileShader(uint32_t *shader, uint32_t type, const char *source);
};

}

#endif
