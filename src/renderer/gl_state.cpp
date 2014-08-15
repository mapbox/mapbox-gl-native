#include <mbgl/renderer/gl_state.hpp>
#include <mbgl/platform/gl.hpp>

#include <cassert>

namespace mbgl {

void GLState::useProgram(const uint32_t program) {
    if (currentProgram != program) {
        glUseProgram(program);
        currentProgram = program;
    }
}

void GLState::lineWidth(const float lineWidth) {
    if (currentLineWidth != lineWidth) {
        glLineWidth(lineWidth);
        currentLineWidth = lineWidth;
    }
}

void GLState::depthMask(const bool value) {
    if (currentDepthMask != value) {
        glDepthMask(value ? GL_TRUE : GL_FALSE);
        currentDepthMask = value;
    }
}

void GLState::depthRange(const std::array<float, 2> &range) {
    if (currentDepthRange != range) {
        glDepthRange(range[0], range[1]);
        currentDepthRange = range;
    }
}

void GLState::viewport(const std::array<uint16_t, 2> &viewport) {
    if (currentViewport != viewport) {
        assert(viewport[0] > 0 && viewport[1] > 0);
        glViewport(0, 0, viewport[0], viewport[1]);
        currentViewport = viewport;
    }
}

const std::array<uint16_t, 2> &GLState::viewport() const {
    return currentViewport;
}


}