#pragma once

#include <mbgl/renderer/renderer_backend.hpp>

namespace mbgl {
namespace android {

class AndroidRendererBackend : public RendererBackend {
public:

    // mbgl::RendererBackend //
    void bind() override;
    void updateAssumedState() override;

    void updateViewPort();

    void resizeFramebuffer(int width, int height);
    const mbgl::Size getFramebufferSize() const;
    PremultipliedImage readFramebuffer() const;

protected:
    // mbgl::RendererBackend //
    gl::ProcAddress initializeExtension(const char*) override;
    void activate() override {};
    void deactivate() override {};


private:

    // Minimum texture size according to OpenGL ES 2.0 specification.
    int fbWidth = 64;
    int fbHeight = 64;

};

} // namespace android
} // namespace mbgl
