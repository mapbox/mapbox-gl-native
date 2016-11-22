#include <mbgl/gl/offscreen_view.hpp>
#include <mbgl/gl/context.hpp>

#include <cstring>
#include <cassert>

namespace mbgl {

OffscreenView::OffscreenView(gl::Context& context_, const Size size_)
    : size(std::move(size_)), context(context_) {
    assert(size);
}

void OffscreenView::bind() {
    if (!framebuffer) {
        color = context.createRenderbuffer<gl::RenderbufferType::RGBA>(size);
        depthStencil = context.createRenderbuffer<gl::RenderbufferType::DepthStencil>(size);
        framebuffer = context.createFramebuffer(*color, *depthStencil);
    } else {
        context.bindFramebuffer = framebuffer->framebuffer;
    }

    context.viewport = { 0, 0, size };
}

PremultipliedImage OffscreenView::readStillImage() {
    return context.readFramebuffer<PremultipliedImage>(size);
}

} // namespace mbgl
