#pragma once

#include <mbgl/gfx/renderable.hpp>
#include <mbgl/gl/renderer_backend.hpp>

namespace mbgl {
namespace android {

class AndroidRendererBackend : public gl::RendererBackend, public mbgl::gfx::Renderable {
public:
    AndroidRendererBackend();
    ~AndroidRendererBackend() override;

    void updateViewPort();

    // Ensures the current context is not cleaned up when destroyed
    void markContextLost();

    void resizeFramebuffer(int width, int height);
    PremultipliedImage readFramebuffer();

    // mbgl::gfx::RendererBackend implementation
public:
    mbgl::gfx::Renderable& getDefaultRenderable() override {
        return *this;
    }

protected:
    void activate() override {
        // no-op
    }
    void deactivate() override {
        // no-op
    }

    // mbgl::gl::RendererBackend implementation
protected:
    mbgl::gl::ProcAddress getExtensionFunctionPointer(const char*) override;
    void updateAssumedState() override;
};

} // namespace android
} // namespace mbgl
