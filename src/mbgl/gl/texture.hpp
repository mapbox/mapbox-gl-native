#pragma once

#include <mbgl/gfx/types.hpp>
#include <mbgl/gl/object.hpp>
#include <mbgl/util/size.hpp>

namespace mbgl {
namespace gl {

class Texture {
public:
    Texture(Size size_, UniqueTexture texture_,
            gfx::TextureFilterType filter_ = gfx::TextureFilterType::Nearest,
            gfx::TextureMipMapType mipmap_ = gfx::TextureMipMapType::No,
            gfx::TextureWrapType wrapX_ = gfx::TextureWrapType::Clamp,
            gfx::TextureWrapType wrapY_ = gfx::TextureWrapType::Clamp)
        : size(std::move(size_)),
          texture(std::move(texture_)),
          filter(filter_),
          mipmap(mipmap_),
          wrapX(wrapX_),
          wrapY(wrapY_) {}

    Size size;
    UniqueTexture texture;
    gfx::TextureFilterType filter;
    gfx::TextureMipMapType mipmap;
    gfx::TextureWrapType wrapX;
    gfx::TextureWrapType wrapY;
};

} // namespace gl
} // namespace mbgl
