#ifndef MBGL_RENDERER_GL_STATE
#define MBGL_RENDERER_GL_STATE

#include <cstdint>
#include <array>

namespace mbgl {

class GLState {
public:
    void useProgram(uint32_t program);
    void lineWidth(float lineWidth);
    void depthMask(bool value);
    void depthRange(const std::array<float, 2> &range);
    void viewport(const std::array<uint16_t, 2> &dimensions);

    const std::array<uint16_t, 2> &viewport() const;

private:
    uint32_t currentProgram = 0;
    float currentLineWidth = 0;
    bool currentDepthMask = true;
    std::array<float, 2> currentDepthRange = {{ 0, 1 }};
    std::array<uint16_t, 2> currentViewport = {{ 0, 0 }};
};

}

#endif
