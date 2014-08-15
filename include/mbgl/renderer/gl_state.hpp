#ifndef MBGL_RENDERER_GL_STATE
#define MBGL_RENDERER_GL_STATE

#include <mbgl/platform/gl.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <cstdint>
#include <array>
#include <cassert>

namespace mbgl {

class GLState : private util::noncopyable {
public:
    inline void useProgram(const uint32_t program) {
        if (currentProgram != program) {
            glUseProgram(program);
            currentProgram = program;
        }
    }

    inline void lineWidth(const float lineWidth) {
        if (currentLineWidth != lineWidth) {
            glLineWidth(lineWidth);
            currentLineWidth = lineWidth;
        }
    }

    inline void depthMask(const bool value) {
        if (currentDepthMask != value) {
            glDepthMask(value ? GL_TRUE : GL_FALSE);
            currentDepthMask = value;
        }
    }

    inline void depthRange(const std::array<float, 2> &range) {
        if (currentDepthRange != range) {
            glDepthRange(range[0], range[1]);
            currentDepthRange = range;
        }
    }

    inline void viewport(const std::array<uint16_t, 2> &viewport) {
        if (currentViewport != viewport) {
            assert(viewport[0] > 0 && viewport[1] > 0);
            glViewport(0, 0, viewport[0], viewport[1]);
            currentViewport = viewport;
        }
    }

    inline void blend(const bool blend) {
        if (currentBlend != blend) {
            blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
            currentBlend = blend;
        }
    }

    inline void depthTest(const bool depthTest) {
        if (currentDepthTest != depthTest) {
            depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
            currentDepthTest = depthTest;
        }
    }

    inline void stencilTest(const bool stencilTest) {
        if (currentStencilTest != stencilTest) {
            stencilTest ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
            currentStencilTest = stencilTest;
        }
    }

    inline void activeTexture(const uint8_t activeTexture) {
        if (currentActiveTexture != activeTexture) {
            // https://www.khronos.org/opengles/sdk/1.1/docs/man/glActiveTexture.xml states that
            // "It is always the case that GL_TEXTUREi = GL_TEXTURE0+i"
            glActiveTexture(GL_TEXTURE0 + activeTexture);
            currentActiveTexture = activeTexture;
        }
    }

    inline void bindTexture(const uint32_t texture) {
        if (currentTexture != texture) {
            glBindTexture(GL_TEXTURE_2D, texture);
            currentTexture = texture;
        }
    }

    inline const std::array<uint16_t, 2> &viewport() const {
        return currentViewport;
    }

private:
    uint32_t currentProgram = 0;
    float currentLineWidth = 0;
    bool currentDepthMask = true;
    std::array<float, 2> currentDepthRange = {{ 0, 1 }};
    std::array<uint16_t, 2> currentViewport = {{ 0, 0 }};
    bool currentBlend = false;
    bool currentDepthTest = false;
    bool currentStencilTest = false;
    uint8_t currentActiveTexture = 0;
    uint32_t currentTexture = 0;
};

}

#endif
