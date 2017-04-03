#include <mbgl/gl/offscreen_view.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/framebuffer.hpp>
#include <mbgl/gl/renderbuffer.hpp>
#include <mbgl/util/optional.hpp>

#include <cstring>
#include <cassert>

namespace mbgl {

class OffscreenView::Impl {
public:
    Impl(gl::Context& context_, const Size size_) : context(context_), size(std::move(size_)) {
        assert(!size.isEmpty());
    }

    void bind() {
        if (!framebuffer) {
            color = context.createRenderbuffer<gl::RenderbufferType::RGBA>(size);
            depthStencil = context.createRenderbuffer<gl::RenderbufferType::DepthStencil>(size);
            framebuffer = context.createFramebuffer(*color, *depthStencil);
        } else {
            context.bindFramebuffer = framebuffer->framebuffer;
        }

        context.viewport = { 0, 0, size };
    }

    PremultipliedImage readStillImage() {
        return context.readFramebuffer<PremultipliedImage>(size);
    }

    const Size& getSize() const {
        return size;
    }

private:
    gl::Context& context;
    const Size size;
    optional<gl::Framebuffer> framebuffer;
    optional<gl::Renderbuffer<gl::RenderbufferType::RGBA>> color;
    optional<gl::Renderbuffer<gl::RenderbufferType::DepthStencil>> depthStencil;
};

OffscreenView::OffscreenView(gl::Context& context, const Size size)
    : impl(std::make_unique<Impl>(context, std::move(size))) {
}

OffscreenView::~OffscreenView() = default;

void OffscreenView::bind() {
    impl->bind();
}

PremultipliedImage OffscreenView::readStillImage() {
    return impl->readStillImage();
}

const Size& OffscreenView::getSize() const {
    return impl->getSize();
}

} // namespace mbgl
