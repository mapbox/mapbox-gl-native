#pragma once

#include <mbgl/gfx/texture.hpp>
#include <mbgl/gl/object.hpp>

namespace mbgl {
namespace gl {

class TextureResource : public gfx::TextureResource {
public:
    TextureResource(UniqueTexture&& texture_) : texture(std::move(texture_)) {
    }

    UniqueTexture texture;
    gfx::TextureFilterType filter = gfx::TextureFilterType::Nearest;
    gfx::TextureMipMapType mipmap = gfx::TextureMipMapType::No;
    gfx::TextureWrapType wrapX = gfx::TextureWrapType::Clamp;
    gfx::TextureWrapType wrapY = gfx::TextureWrapType::Clamp;
};

} // namespace gl
} // namespace mbgl
