#pragma once

#include <mbgl/gfx/offscreen_texture.hpp>
#include <mbgl/gfx/types.hpp>

namespace mbgl {
namespace gl {

class Context;

class OffscreenTexture final : public gfx::OffscreenTexture {
public:
    OffscreenTexture(gl::Context&,
                     Size size,
                     gfx::TextureChannelDataType type = gfx::TextureChannelDataType::UnsignedByte);

    bool isRenderable() override;

    PremultipliedImage readStillImage() override;
    gfx::Texture& getTexture() override;
};

} // namespace gl
} // namespace mbgl
