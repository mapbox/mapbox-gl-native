#pragma once

#include <mbgl/util/size.hpp>

namespace mbgl {
namespace gfx {
class RendererBackend;
} // namespace gfx
} // namespace mbgl

class GLFWBackend {
public:
    explicit GLFWBackend() = default;
    GLFWBackend(const GLFWBackend&) = delete;
    GLFWBackend& operator=(const GLFWBackend&) = delete;
    virtual ~GLFWBackend() = default;

    virtual mbgl::gfx::RendererBackend& getRendererBackend() = 0;
    virtual mbgl::Size getSize() const = 0;
    virtual void setSize(mbgl::Size) = 0;
};
