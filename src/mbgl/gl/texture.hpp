#pragma once

#include <mbgl/gl/object.hpp>
#include <mbgl/util/size.hpp>

namespace mbgl {
namespace gl {

class Texture {
public:
    Size size;
    UniqueTexture texture;
    TextureFilter filter = TextureFilter::Nearest;
    TextureMipMap mipmap = TextureMipMap::No;
    TextureWrap wrapX = TextureWrap::Clamp;
    TextureWrap wrapY = TextureWrap::Clamp;
};

} // namespace gl
} // namespace mbgl
