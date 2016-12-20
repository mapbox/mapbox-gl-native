#pragma once

#include <mbgl/gl/object.hpp>
#include <mbgl/util/size.hpp>

#include <functional>

namespace mbgl {
namespace gl {

class Texture {
public:
    Texture(Size size_, std::function<UniqueTexture()> getTexture) : size(size_), texture(getTexture()) {}

    Size size;
    UniqueTexture texture;
    TextureFilter filter = TextureFilter::Nearest;
    TextureMipMap mipmap = TextureMipMap::No;
    TextureWrap wrapX = TextureWrap::Clamp;
    TextureWrap wrapY = TextureWrap::Clamp;
};

} // namespace gl
} // namespace mbgl
