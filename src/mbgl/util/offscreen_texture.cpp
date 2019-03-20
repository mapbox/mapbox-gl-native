#include <mbgl/gl/context.hpp>
#include <mbgl/util/offscreen_texture.hpp>

#include <cassert>
#include <cstring>

namespace mbgl {

OffscreenTexture::OffscreenTexture(OffscreenTexture&&) = default;
OffscreenTexture& OffscreenTexture::operator=(OffscreenTexture&&) = default;

class OffscreenTexture::Impl {
public:
    Impl(gl::Context& context_, const Size size_, const gfx::TextureChannelDataType type_)
        : context(context_), size(std::move(size_)), type(type_) {
        assert(!size.isEmpty());
    }
    Impl(gl::Context& context_,
         const Size size_,
         gfx::Renderbuffer<gfx::RenderbufferPixelType::Depth>& depth_,
         const gfx::TextureChannelDataType type_)
        : context(context_), size(std::move(size_)), depth(&depth_), type(type_) {
        assert(!size.isEmpty());
    }

    void bind() {
        if (!framebuffer) {
            texture = context.createTexture(size, gfx::TexturePixelType::RGBA, type);
            if (depth) {
                framebuffer = context.createFramebuffer(*texture, *depth);
            } else {
                framebuffer = context.createFramebuffer(*texture);
            }
        } else {
            context.bindFramebuffer = framebuffer->framebuffer;
        }

        context.activeTextureUnit = 0;
        context.scissorTest = false;
        context.viewport = { 0, 0, size };
    }

    PremultipliedImage readStillImage() {
        return context.readFramebuffer<PremultipliedImage>(size);
    }

    gfx::Texture& getTexture() {
        assert(texture);
        return *texture;
    }

    const Size& getSize() const {
        return size;
    }

private:
    gl::Context& context;
    const Size size;
    optional<gl::Framebuffer> framebuffer;
    optional<gfx::Texture> texture;
    gfx::Renderbuffer<gfx::RenderbufferPixelType::Depth>* depth = nullptr;
    const gfx::TextureChannelDataType type;
};

OffscreenTexture::OffscreenTexture(gfx::Context& context,
                                   const Size size,
                                   const gfx::TextureChannelDataType type)
    // TODO: remove cast
    : impl(std::make_unique<Impl>(static_cast<gl::Context&>(context), std::move(size), type)) {
    assert(!size.isEmpty());
}

OffscreenTexture::OffscreenTexture(gfx::Context& context,
                                   const Size size,
                                   gfx::Renderbuffer<gfx::RenderbufferPixelType::Depth>& renderbuffer,
                                   const gfx::TextureChannelDataType type)
    // TODO: remove cast
    : impl(std::make_unique<Impl>(static_cast<gl::Context&>(context), std::move(size), renderbuffer, type)) {
    assert(!size.isEmpty());
}

OffscreenTexture::~OffscreenTexture() = default;

void OffscreenTexture::bind() {
    impl->bind();
}

PremultipliedImage OffscreenTexture::readStillImage() {
    return impl->readStillImage();
}

gfx::Texture& OffscreenTexture::getTexture() {
    return impl->getTexture();
}

const Size& OffscreenTexture::getSize() const {
    return impl->getSize();
}

} // namespace mbgl
