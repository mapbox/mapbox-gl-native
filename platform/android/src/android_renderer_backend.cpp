#include "android_renderer_backend.hpp"

#include <mbgl/gl/context.hpp>

#include <EGL/egl.h>

#include <cassert>

namespace mbgl {
namespace android {

/**
 * From mbgl::View
 */
void AndroidRendererBackend::bind() {
    assert(BackendScope::exists());
    setFramebufferBinding(0);
    setViewport(0, 0, getFramebufferSize());
}

/**
 * From mbgl::RendererBackend.
 */
gl::ProcAddress AndroidRendererBackend::getExtensionFunctionPointer(const char* name) {
    assert(BackendScope::exists());
    return eglGetProcAddress(name);
}

void AndroidRendererBackend::updateViewPort() {
    assert(BackendScope::exists());
    setViewport(0, 0, getFramebufferSize());
}

void AndroidRendererBackend::resizeFramebuffer(int width, int height) {
    fbWidth = width;
    fbHeight = height;
}

PremultipliedImage AndroidRendererBackend::readFramebuffer() const {
    assert(BackendScope::exists());
    return RendererBackend::readFramebuffer(getFramebufferSize());
}

mbgl::Size AndroidRendererBackend::getFramebufferSize() const {
    return { static_cast<uint32_t>(fbWidth), static_cast<uint32_t>(fbHeight) };
}

/**
 * From mbgl::RendererBackend.
 */
void AndroidRendererBackend::updateAssumedState() {
    assumeFramebufferBinding(0);
    assumeViewport(0, 0, getFramebufferSize());
}

void AndroidRendererBackend::markContextLost() {
    if (context) {
        context->setCleanupOnDestruction(false);
    }
}

} // namespace android
} // namspace mbgl
