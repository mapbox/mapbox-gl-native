#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/gl/renderbuffer.hpp>
#include <mbgl/util/image.hpp>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

namespace gfx {
class Texture;
} // namespace gfx

class OffscreenTexture {
public:
    OffscreenTexture(gl::Context&,
                     Size size = { 256, 256 },
                     gfx::TextureChannelDataType type = gfx::TextureChannelDataType::UnsignedByte);
    OffscreenTexture(gl::Context&,
                     Size size,
                     gl::Renderbuffer<gl::RenderbufferType::DepthComponent>&,
                     gfx::TextureChannelDataType type = gfx::TextureChannelDataType::UnsignedByte);
    ~OffscreenTexture();
    OffscreenTexture(OffscreenTexture&&);
    OffscreenTexture& operator=(OffscreenTexture&&);

    void bind();

    PremultipliedImage readStillImage();

    gfx::Texture& getTexture();

    const Size& getSize() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace mbgl
