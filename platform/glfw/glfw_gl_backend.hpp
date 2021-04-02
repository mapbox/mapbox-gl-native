#pragma once

#include "glfw_backend.hpp"

#include <mbgl/gfx/renderable.hpp>
#include <mbgl/gl/renderer_backend.hpp>

struct GLFWwindow;

class GLFWGLBackend final : public GLFWBackend,
                            public mbgl::gl::RendererBackend,
                            public mbgl::gfx::Renderable {
public:
    GLFWGLBackend(GLFWwindow*, bool capFrameRate);
    ~GLFWGLBackend() override;

    void swap();

    // GLFWRendererBackend implementation
    auto getRendererBackend() -> mbgl::gfx::RendererBackend& override {
        return *this;
    }
    auto getSize() const -> mbgl::Size override;
    void setSize(mbgl::Size) override;

    // mbgl::gfx::RendererBackend implementation
    auto getDefaultRenderable() -> mbgl::gfx::Renderable& override {
        return *this;
    }

protected:
    void activate() override;
    void deactivate() override;

    // mbgl::gl::RendererBackend implementation
    auto getExtensionFunctionPointer(const char*) -> mbgl::gl::ProcAddress override;
    void updateAssumedState() override;

private:
    GLFWwindow* window;
};
