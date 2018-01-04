#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/renderer/backend_scope.hpp>

#include <cassert>
#include <stdexcept>
#include <type_traits>

namespace mbgl {

class HeadlessBackend::View {
public:
    View(gl::Context& context, Size size_)
        : color(context.createRenderbuffer<gl::RenderbufferType::RGBA>(size_)),
          depthStencil(context.createRenderbuffer<gl::RenderbufferType::DepthStencil>(size_)),
          framebuffer(context.createFramebuffer(color, depthStencil)) {
    }

    gl::Renderbuffer<gl::RenderbufferType::RGBA> color;
    gl::Renderbuffer<gl::RenderbufferType::DepthStencil> depthStencil;
    gl::Framebuffer framebuffer;
};

HeadlessBackend::HeadlessBackend(Size size_)
    : size(size_) {
}

HeadlessBackend::~HeadlessBackend() {
    BackendScope guard { *this };
    view.reset();
    context.reset();
}

gl::ProcAddress HeadlessBackend::getExtensionFunctionPointer(const char* name) {
    assert(impl);
    return impl->getExtensionFunctionPointer(name);
}

void HeadlessBackend::activate() {
    active = true;

    if (!impl) {
        createImpl();
    }

    assert(impl);
    impl->activateContext();
}

void HeadlessBackend::deactivate() {
    assert(impl);
    impl->deactivateContext();
    active = false;
}

void HeadlessBackend::bind() {
    gl::Context& context_ = getContext();

    if (!view) {
        view = std::make_unique<View>(context_, size);
    }

    context_.bindFramebuffer = view->framebuffer.framebuffer;
    context_.scissorTest = false;
    context_.viewport = { 0, 0, size };
}

Size HeadlessBackend::getFramebufferSize() const {
    return size;
}

void HeadlessBackend::updateAssumedState() {
    // no-op
}

void HeadlessBackend::setSize(Size size_) {
    size = size_;
    view.reset();
}

PremultipliedImage HeadlessBackend::readStillImage() {
    return getContext().readFramebuffer<PremultipliedImage>(size);
}

} // namespace mbgl
