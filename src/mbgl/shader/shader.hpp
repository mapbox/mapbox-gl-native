#ifndef MBGL_RENDERER_SHADER
#define MBGL_RENDERER_SHADER

#include <cstdint>
#include <array>
#include <string>
#include <mbgl/util/noncopyable.hpp>

namespace mbgl {
namespace gl {
class Config;
} // namespace gl

class Shader : private util::noncopyable {
public:
    typedef void (*BindIndices)(uint32_t);

    Shader(const char *name, const char *vertex, const char *fragment, BindIndices = nullptr);
    ~Shader();
    const char* const name;
    uint32_t program;

    virtual void bind(gl::Config& config, char *offset) = 0;

    inline uint32_t getID() const {
        return program;
    }

private:
    bool compileShader(uint32_t *shader, uint32_t type, const char *source);
};

}

#endif
