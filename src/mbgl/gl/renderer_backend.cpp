#include <mbgl/gl/renderer_backend.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/extension.hpp>

#include <cassert>

namespace mbgl {
namespace gl {

RendererBackend::RendererBackend(const gfx::ContextMode contextMode_)
    : gfx::RendererBackend(contextMode_) {
}

std::unique_ptr<gfx::Context> RendererBackend::createContext() {
    auto result = std::make_unique<gl::Context>(*this);
    result->enableDebugging();
    result->initializeExtensions(
            std::bind(&RendererBackend::getExtensionFunctionPointer, this, std::placeholders::_1));
    // Needs move to placate GCC 4.9
    return std::move(result);
}

gl::Context& RendererBackend::getGLContext() {
    return static_cast<gl::Context&>(getContext());
}

PremultipliedImage RendererBackend::readFramebuffer(const Size& size) {
    return getGLContext().readFramebuffer<PremultipliedImage>(size);
}

void RendererBackend::assumeFramebufferBinding(const gl::FramebufferID fbo) {
    getGLContext().bindFramebuffer.setCurrentValue(fbo);
    if (fbo != ImplicitFramebufferBinding) {
        assert(gl::value::BindFramebuffer::Get() == getGLContext().bindFramebuffer.getCurrentValue());
    }
}

void RendererBackend::assumeViewport(int32_t x, int32_t y, const Size& size) {
    getGLContext().viewport.setCurrentValue({ x, y, size });
    assert(gl::value::Viewport::Get() == getGLContext().viewport.getCurrentValue());
}

void RendererBackend::assumeScissorTest(bool enabled) {
    getGLContext().scissorTest.setCurrentValue(enabled);
    assert(gl::value::ScissorTest::Get() == getGLContext().scissorTest.getCurrentValue());
}

bool RendererBackend::implicitFramebufferBound() {
    return getGLContext().bindFramebuffer.getCurrentValue() == ImplicitFramebufferBinding;
}

void RendererBackend::setFramebufferBinding(const gl::FramebufferID fbo) {
    getGLContext().bindFramebuffer = fbo;
    if (fbo != ImplicitFramebufferBinding) {
        assert(gl::value::BindFramebuffer::Get() == getGLContext().bindFramebuffer.getCurrentValue());
    }
}

void RendererBackend::setViewport(int32_t x, int32_t y, const Size& size) {
    getGLContext().viewport = { x, y, size };
    assert(gl::value::Viewport::Get() == getGLContext().viewport.getCurrentValue());
}

void RendererBackend::setScissorTest(bool enabled) {
    getGLContext().scissorTest = enabled;
    assert(gl::value::ScissorTest::Get() == getGLContext().scissorTest.getCurrentValue());
}

RendererBackend::~RendererBackend() = default;

} // namespace gl
} // namespace mbgl
