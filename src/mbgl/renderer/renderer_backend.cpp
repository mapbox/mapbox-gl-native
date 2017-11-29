#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/extension.hpp>
#include <mbgl/gl/debugging.hpp>

#include <cassert>

namespace mbgl {

RendererBackend::RendererBackend() = default;

gl::Context& RendererBackend::getContext() {
    assert(BackendScope::exists());
    std::call_once(initialized, [this] {
        context = std::make_unique<gl::Context>();
        context->enableDebugging();
        context->initializeExtensions(
            std::bind(&RendererBackend::getExtensionFunctionPointer, this, std::placeholders::_1));
    });
    return *context;
}

PremultipliedImage RendererBackend::readFramebuffer(const Size& size) const {
    assert(context);
    return context->readFramebuffer<PremultipliedImage>(size);
}

void RendererBackend::assumeFramebufferBinding(const gl::FramebufferID fbo) {
    getContext().bindFramebuffer.setCurrentValue(fbo);
    if (fbo != ImplicitFramebufferBinding) {
        assert(gl::value::BindFramebuffer::Get() == getContext().bindFramebuffer.getCurrentValue());
    }
}

void RendererBackend::assumeViewport(int32_t x, int32_t y, const Size& size) {
    getContext().viewport.setCurrentValue({ x, y, size });
    assert(gl::value::Viewport::Get() == getContext().viewport.getCurrentValue());
}

void RendererBackend::assumeScissorTest(bool enabled) {
    getContext().scissorTest.setCurrentValue(enabled);
    assert(gl::value::ScissorTest::Get() == getContext().scissorTest.getCurrentValue());
}

bool RendererBackend::implicitFramebufferBound() {
    return getContext().bindFramebuffer.getCurrentValue() == ImplicitFramebufferBinding;
}

void RendererBackend::setFramebufferBinding(const gl::FramebufferID fbo) {
    getContext().bindFramebuffer = fbo;
    if (fbo != ImplicitFramebufferBinding) {
        assert(gl::value::BindFramebuffer::Get() == getContext().bindFramebuffer.getCurrentValue());
    }
}

void RendererBackend::setViewport(int32_t x, int32_t y, const Size& size) {
    getContext().viewport = { x, y, size };
    assert(gl::value::Viewport::Get() == getContext().viewport.getCurrentValue());
}

void RendererBackend::setScissorTest(bool enabled) {
    getContext().scissorTest = enabled;
    assert(gl::value::ScissorTest::Get() == getContext().scissorTest.getCurrentValue());
}

RendererBackend::~RendererBackend() = default;

} // namespace mbgl
