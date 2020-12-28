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
public:
    mbgl::gfx::RendererBackend& getRendererBackend() override {
        return *this;
    }
    mbgl::Size getSize() const override;
    void setSize(mbgl::Size) override;

    // mbgl::gfx::RendererBackend implementation
public:
    mbgl::gfx::Renderable& getDefaultRenderable() override {
        return *this;
    }

protected:
    void activate() override;
    void deactivate() override;

    // mbgl::gl::RendererBackend implementation
protected:
    mbgl::gl::ProcAddress getExtensionFunctionPointer(const char*) override;
    void updateAssumedState() override;

private:
    GLFWwindow* window;
};
