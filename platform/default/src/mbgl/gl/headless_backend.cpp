#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/renderable_resource.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gfx/backend_scope.hpp>

#include <cassert>
#include <stdexcept>
#include <type_traits>

namespace mbgl {
namespace gl {

class HeadlessRenderableResource final : public gl::RenderableResource {
public:
    HeadlessRenderableResource(HeadlessBackend& backend_, gl::Context& context_, Size size_)
        : backend(backend_),
          context(context_),
          color(context.createRenderbuffer<gfx::RenderbufferPixelType::RGBA>(size_)),
          depthStencil(context.createRenderbuffer<gfx::RenderbufferPixelType::DepthStencil>(size_)),
          framebuffer(context.createFramebuffer(color, depthStencil)) {}

    void bind() override {
        context.bindFramebuffer = framebuffer.framebuffer;
        context.scissorTest = false;
        context.viewport = { 0, 0, framebuffer.size };
    }

    void swap() override { backend.swap(); }

    HeadlessBackend& backend;
    gl::Context& context;
    gfx::Renderbuffer<gfx::RenderbufferPixelType::RGBA> color;
    gfx::Renderbuffer<gfx::RenderbufferPixelType::DepthStencil> depthStencil;
    gl::Framebuffer framebuffer;
};

HeadlessBackend::HeadlessBackend(const Size size_,
                                 gfx::HeadlessBackend::SwapBehaviour swapBehaviour_,
                                 const gfx::ContextMode contextMode_)
    : mbgl::gl::RendererBackend(contextMode_), mbgl::gfx::HeadlessBackend(size_), swapBehaviour(swapBehaviour_) {}

HeadlessBackend::~HeadlessBackend() {
    gfx::BackendScope guard{*this};
    resource.reset();
    // Explicitly reset the context so that it is destructed and cleaned up before we destruct
    // the impl object.
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

gfx::Renderable& HeadlessBackend::getDefaultRenderable() {
    if (!resource) {
        resource = std::make_unique<HeadlessRenderableResource>(*this, static_cast<gl::Context&>(getContext()), size);
    }
    return *this;
}

void HeadlessBackend::swap() {
    if (swapBehaviour == SwapBehaviour::Flush) static_cast<gl::Context&>(getContext()).finish();
}

void HeadlessBackend::updateAssumedState() {
    // no-op
}

PremultipliedImage HeadlessBackend::readStillImage() {
    return static_cast<gl::Context&>(getContext()).readFramebuffer<PremultipliedImage>(size);
}

RendererBackend* HeadlessBackend::getRendererBackend() {
    return this;
}

} // namespace gl

namespace gfx {

template <>
std::unique_ptr<gfx::HeadlessBackend> Backend::Create<gfx::Backend::Type::OpenGL>(
    const Size size, gfx::HeadlessBackend::SwapBehaviour swapBehavior, const gfx::ContextMode contextMode) {
    return std::make_unique<gl::HeadlessBackend>(size, swapBehavior, contextMode);
}

} // namespace gfx
} // namespace mbgl


