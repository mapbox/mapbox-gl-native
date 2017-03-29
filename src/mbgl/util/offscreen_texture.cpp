#include <mbgl/gl/context.hpp>
#include <mbgl/util/offscreen_texture.hpp>

#include <cassert>
#include <cstring>

namespace mbgl {

class OffscreenTexture::Impl {
public:
    Impl(gl::Context& context_, const Size size_) : context(context_), size(std::move(size_)) {
        assert(!size.isEmpty());
    }

    void bind() {
        if (!framebuffer) {
            texture = context.createTexture(size, gl::TextureFormat::RGBA, unit);
            framebuffer = context.createFramebuffer(*texture);
        } else {
            context.bindFramebuffer = framebuffer->framebuffer;
        }

        context.activeTexture = unit;
        context.viewport = { 0, 0, size };
    }

    PremultipliedImage readStillImage() {
        return context.readFramebuffer<PremultipliedImage>(size);
    }

    gl::Texture& getTexture() {
        assert(texture);
        return *texture;
    }

    const Size& getSize() const {
        return size;
    }

    void bindRenderbuffers() {
        if (!framebuffer) {
            texture = context.createTexture(size, gl::TextureFormat::RGBA, unit);
            colorTarget = context.createRenderbuffer<gl::RenderbufferType::RGBA4>(size);
            depthTarget = context.createRenderbuffer<gl::RenderbufferType::DepthComponent>(size);
            framebuffer = context.createFramebuffer(*colorTarget, *depthTarget, *texture);
        } else {
            context.bindFramebuffer = framebuffer->framebuffer;
        }

        context.viewport = { 0, 0, size };
    }

private:
    gl::Context& context;
    const Size size;
    gl::TextureUnit unit;
    optional<gl::Framebuffer> framebuffer;
    optional<gl::Texture> texture;
    optional<gl::Renderbuffer<gl::RenderbufferType::RGBA4>> colorTarget;
    optional<gl::Renderbuffer<gl::RenderbufferType::DepthComponent>> depthTarget;
};

OffscreenTexture::OffscreenTexture(gl::Context& context, const Size size)
    : impl(std::make_unique<Impl>(context, std::move(size))) {
    assert(!size.isEmpty());
}

OffscreenTexture::~OffscreenTexture() = default;

void OffscreenTexture::bind() {
    impl->bind();
}

PremultipliedImage OffscreenTexture::readStillImage() {
    return impl->readStillImage();
}

gl::Texture& OffscreenTexture::getTexture() {
    return impl->getTexture();
}

const Size& OffscreenTexture::getSize() const {
    return impl->getSize();
}

void OffscreenTexture::bindRenderbuffers() {
    impl->bindRenderbuffers();
}

} // namespace mbgl
