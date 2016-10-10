#include <mbgl/platform/default/offscreen_view.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/gl.hpp>

#include <cstring>
#include <cassert>

namespace mbgl {

OffscreenView::OffscreenView(gl::Context& context_, std::array<uint16_t, 2> size_)
    : context(context_), size(std::move(size_)) {
    assert(size[0] > 0 && size[1] > 0);
}

void OffscreenView::bind() {
    if (!framebuffer) {
        color = context.createRenderbuffer<gl::RenderbufferType::RGBA>(size);
        depthStencil = context.createRenderbuffer<gl::RenderbufferType::DepthStencil>(size);
        framebuffer = context.createFramebuffer(*color, *depthStencil);
    } else {
        context.bindFramebuffer = framebuffer->framebuffer;
    }

    context.viewport = { 0, 0, size[0], size[1] };
}

PremultipliedImage OffscreenView::readStillImage() {
    PremultipliedImage image { size[0], size[1] };
    MBGL_CHECK_ERROR(glReadPixels(0, 0, size[0], size[1], GL_RGBA, GL_UNSIGNED_BYTE, image.data.get()));

    const auto stride = image.stride();
    auto tmp = std::make_unique<uint8_t[]>(stride);
    uint8_t* rgba = image.data.get();
    for (int i = 0, j = size[1] - 1; i < j; i++, j--) {
        std::memcpy(tmp.get(), rgba + i * stride, stride);
        std::memcpy(rgba + i * stride, rgba + j * stride, stride);
        std::memcpy(rgba + j * stride, tmp.get(), stride);
    }

    return image;
}

std::array<uint16_t, 2> OffscreenView::getSize() const {
    return size;
}

} // namespace mbgl
