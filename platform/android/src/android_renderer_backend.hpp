#pragma once

#include <mbgl/renderer/renderer_backend.hpp>

namespace mbgl {
namespace android {

class AndroidRendererBackend : public RendererBackend {
public:

    // mbgl::RendererBackend //
    void bind() override;
    void updateAssumedState() override;
    mbgl::Size getFramebufferSize() const override;

    // Ensures the current context is not
    // cleaned up when destroyed
    void markContextLost();

    void updateViewPort();

    void resizeFramebuffer(int width, int height);
    PremultipliedImage readFramebuffer() const;

protected:
    // mbgl::RendererBackend //
    gl::ProcAddress getExtensionFunctionPointer(const char*) override;
    void activate() override {};
    void deactivate() override {};


private:

    // Minimum texture size according to OpenGL ES 2.0 specification.
    int fbWidth = 64;
    int fbHeight = 64;

};

} // namespace android
} // namespace mbgl
