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
};

} // namespace gl
} // namespace mbgl
