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
};

} // namespace gl
} // namespace mbgl
