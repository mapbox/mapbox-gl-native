#include "android_renderer_backend.hpp"

#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/renderable_resource.hpp>

#include <EGL/egl.h>

#include <cassert>

namespace mbgl {
namespace android {

class AndroidGLRenderableResource final : public mbgl::gl::RenderableResource {
public:
    AndroidGLRenderableResource(AndroidRendererBackend& backend_) : backend(backend_) {
    }

    void bind() override {
        assert(gfx::BackendScope::exists());
        backend.setFramebufferBinding(0);
        backend.setViewport(0, 0, backend.getSize());
    }

private:
    AndroidRendererBackend& backend;
};

AndroidRendererBackend::AndroidRendererBackend()
    : gl::RendererBackend(gfx::ContextMode::Unique),
      mbgl::gfx::Renderable({ 64, 64 }, std::make_unique<AndroidGLRenderableResource>(*this)) {
}

AndroidRendererBackend::~AndroidRendererBackend() = default;

gl::ProcAddress AndroidRendererBackend::getExtensionFunctionPointer(const char* name) {
    assert(gfx::BackendScope::exists());
    return eglGetProcAddress(name);
}

void AndroidRendererBackend::updateViewPort() {
    assert(gfx::BackendScope::exists());
    setViewport(0, 0, size);
}

void AndroidRendererBackend::resizeFramebuffer(int width, int height) {
    size = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
}

PremultipliedImage AndroidRendererBackend::readFramebuffer() {
    assert(gfx::BackendScope::exists());
    return gl::RendererBackend::readFramebuffer(size);
}

void AndroidRendererBackend::updateAssumedState() {
    assumeFramebufferBinding(0);
    assumeViewport(0, 0, size);
}

void AndroidRendererBackend::markContextLost() {
    if (context) {
        getContext<gl::Context>().setCleanupOnDestruction(false);
    }
}

} // namespace android
} // namspace mbgl
