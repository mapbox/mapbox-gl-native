#include <mbgl/map/backend.hpp>
#include <mbgl/map/backend_scope.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/extension.hpp>
#include <mbgl/gl/debugging.hpp>

#include <cassert>

namespace mbgl {

Backend::Backend() = default;

gl::Context& Backend::getContext() {
    assert(BackendScope::exists());
    std::call_once(initialized, [this] {
        context = std::make_unique<gl::Context>();
        context->enableDebugging();
        context->initializeExtensions(
            std::bind(&Backend::initializeExtension, this, std::placeholders::_1));
    });
    return *context;
}

PremultipliedImage Backend::readFramebuffer(const Size& size) const {
    assert(context);
    return context->readFramebuffer<PremultipliedImage>(size);
}

void Backend::assumeFramebufferBinding(const gl::FramebufferID fbo) {
    getContext().bindFramebuffer.setCurrentValue(fbo);
    if (fbo != ImplicitFramebufferBinding) {
        assert(gl::value::BindFramebuffer::Get() == getContext().bindFramebuffer.getCurrentValue());
    }
}
void Backend::assumeViewportSize(const Size& size) {
    getContext().viewport.setCurrentValue({ 0, 0, size });
    assert(gl::value::Viewport::Get() == getContext().viewport.getCurrentValue());
}

bool Backend::implicitFramebufferBound() {
    return getContext().bindFramebuffer.getCurrentValue() == ImplicitFramebufferBinding;
}

void Backend::setFramebufferBinding(const gl::FramebufferID fbo) {
    getContext().bindFramebuffer = fbo;
    if (fbo != ImplicitFramebufferBinding) {
        assert(gl::value::BindFramebuffer::Get() == getContext().bindFramebuffer.getCurrentValue());
    }
}

void Backend::setViewportSize(const Size& size) {
    getContext().viewport = { 0, 0, size };
    assert(gl::value::Viewport::Get() == getContext().viewport.getCurrentValue());
}

Backend::~Backend() = default;

} // namespace mbgl
