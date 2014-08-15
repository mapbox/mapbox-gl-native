#ifndef MBGL_RENDERER_SHADER
#define MBGL_RENDERER_SHADER

#include <cstdint>
#include <array>
#include <mbgl/util/noncopyable.hpp>

namespace mbgl {

class Shader : private util::noncopyable {
public:
    Shader(const char *name, const char *vertex, const char *fragment);
    ~Shader();
    const char *name;
    bool valid;
    uint32_t program;

    void setMatrix(const std::array<float, 16>& matrix);

    inline uint32_t getID() const {
        return program;
    }

private:
    bool compileShader(uint32_t *shader, uint32_t type, const char *source);

protected:
    std::array<float, 16> matrix = {{}};
    int32_t u_matrix = -1;
};

}

#endif
