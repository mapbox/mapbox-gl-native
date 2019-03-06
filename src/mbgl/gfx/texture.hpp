#pragma once

#include <mbgl/gfx/types.hpp>
#include <mbgl/util/size.hpp>

#include <memory>

namespace mbgl {
namespace gfx {

class TextureResource {
protected:
    TextureResource() = default;
public:
    virtual ~TextureResource() = default;
};

class Texture {
public:
    Texture(Size size_, std::unique_ptr<const TextureResource>&& resource_)
        : size(std::move(size_)), resource(std::move(resource_)) {
    }

    Size size;
    TextureFilterType filter = TextureFilterType::Nearest;
    TextureMipMapType mipmap = TextureMipMapType::No;
    TextureWrapType wrapX = TextureWrapType::Clamp;
    TextureWrapType wrapY = TextureWrapType::Clamp;
    std::unique_ptr<const TextureResource> resource;
};

} // namespace gfx
} // namespace mbgl
