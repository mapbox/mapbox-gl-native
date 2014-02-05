#ifndef LLMR_RENDERER_SHADER
#define LLMR_RENDERER_SHADER

#include <cstdint>

namespace llmr {

class Shader {
public:
    Shader(const char *vertex, const char *fragment);
    virtual ~Shader();

    // Make noncopyable
    Shader(const Shader&) = delete;
    Shader(const Shader&&) = delete;
    Shader &operator=(const Shader&) = delete;
    Shader &operator=(const Shader&&) = delete;

    bool valid;
    uint32_t program;

private:
    bool compileShader(uint32_t *shader, uint32_t type, const char *source);
};

}

#endif
