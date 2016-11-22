#pragma once

#include <mbgl/gl/object.hpp>

#include <array>

namespace mbgl {
namespace gl {

class Texture {
public:
    std::array<uint16_t, 2> size;
    UniqueTexture texture;
    TextureFilter filter = TextureFilter::Nearest;
    TextureMipMap mipmap = TextureMipMap::No;
};

} // namespace gl
} // namespace mbgl
