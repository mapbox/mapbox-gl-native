#ifndef LLMR_RENDERER_SHADER
#define LLMR_RENDERER_SHADER

#include <vector>

namespace llmr {

class Shader {
public:
    Shader(const char *vertex, const char *fragment);
    ~Shader();
    bool valid;
    uint32_t program;
    std::vector<uint32_t> attributes;

private:
    bool compileShader(uint32_t *shader, uint32_t type, const char *source);
};

}

#endif
