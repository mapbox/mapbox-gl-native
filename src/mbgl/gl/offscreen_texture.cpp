#include <mbgl/gl/offscreen_texture.hpp>
#include <mbgl/gl/renderable_resource.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/framebuffer.hpp>

namespace mbgl {
namespace gl {

class OffscreenTextureResource final : public gl::RenderableResource {
public:
    OffscreenTextureResource(gl::Context& context_,
                             const Size size_,
                             const gfx::TextureChannelDataType type_)
        : context(context_), size(size_), type(type_) {
        assert(!size.isEmpty());
    }

    OffscreenTextureResource(gl::Context& context_,
                             const Size size_,
                             gfx::Renderbuffer<gfx::RenderbufferPixelType::Depth>& depth_,
                             const gfx::TextureChannelDataType type_)
        : context(context_), size(size_), depth(&depth_), type(type_) {
        assert(!size.isEmpty());
    }

    void bind() override {
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
        assert(framebuffer);
        context.bindFramebuffer = framebuffer->framebuffer;
        return context.readFramebuffer<PremultipliedImage>(size);
    }

    gfx::Texture& getTexture() {
        assert(texture);
        return *texture;
    }

private:
    gl::Context& context;
    const Size size;
    optional<gfx::Texture> texture;
    gfx::Renderbuffer<gfx::RenderbufferPixelType::Depth>* depth = nullptr;
    const gfx::TextureChannelDataType type;
    optional<gl::Framebuffer> framebuffer;
};

OffscreenTexture::OffscreenTexture(gl::Context& context,
                                   const Size size_,
                                   const gfx::TextureChannelDataType type)
    : gfx::OffscreenTexture(size, std::make_unique<OffscreenTextureResource>(context, size_, type)) {
}

OffscreenTexture::OffscreenTexture(
    gl::Context& context,
    const Size size_,
    gfx::Renderbuffer<gfx::RenderbufferPixelType::Depth>& renderbuffer,
    const gfx::TextureChannelDataType type)
    : gfx::OffscreenTexture(
          size, std::make_unique<OffscreenTextureResource>(context, size_, renderbuffer, type)) {
}

bool OffscreenTexture::isRenderable() {
    try {
        getResource<OffscreenTextureResource>().bind();
        return true;
    } catch (const std::runtime_error& ex) {
        return false;
    }
}

PremultipliedImage OffscreenTexture::readStillImage() {
    return getResource<OffscreenTextureResource>().readStillImage();
}

gfx::Texture& OffscreenTexture::getTexture() {
    return getResource<OffscreenTextureResource>().getTexture();
}

} // namespace gl
} // namespace mbgl
