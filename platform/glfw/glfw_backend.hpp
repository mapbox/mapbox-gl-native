#pragma once

#include <mbgl/util/size.hpp>
#include <mbgl/gfx/backend.hpp>

namespace mbgl {
namespace gfx {
class RendererBackend;
} // namespace gfx
} // namespace mbgl

struct GLFWwindow;

class GLFWBackend {
public:
    explicit GLFWBackend() = default;
    GLFWBackend(const GLFWBackend&) = delete;
    auto operator=(const GLFWBackend&) -> GLFWBackend& = delete;
    virtual ~GLFWBackend() = default;

    static auto Create(GLFWwindow* window, bool capFrameRate) -> std::unique_ptr<GLFWBackend> {
        return mbgl::gfx::Backend::Create<GLFWBackend, GLFWwindow*, bool>(window, capFrameRate);
    }

    virtual auto getRendererBackend() -> mbgl::gfx::RendererBackend& = 0;
    virtual auto getSize() const -> mbgl::Size = 0;
    virtual void setSize(mbgl::Size) = 0;
};
