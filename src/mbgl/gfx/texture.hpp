#pragma once

#include <mbgl/gfx/types.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/util/type_list.hpp>
#include <mbgl/util/indexed_tuple.hpp>

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

class TextureBinding {
public:
    TextureBinding(const TextureResource& resource_,
                   TextureFilterType filter_ = TextureFilterType::Nearest,
                   TextureMipMapType mipmap_ = TextureMipMapType::No,
                   TextureWrapType wrapX_ = TextureWrapType::Clamp,
                   TextureWrapType wrapY_ = TextureWrapType::Clamp)
        : resource(&resource_), filter(filter_), mipmap(mipmap_), wrapX(wrapX_), wrapY(wrapY_) {
    }

    const TextureResource* resource;
    TextureFilterType filter;
    TextureMipMapType mipmap;
    TextureWrapType wrapX;
    TextureWrapType wrapY;
};

namespace detail {

template <class>
class TextureBindings;

template <class... Ts>
class TextureBindings<TypeList<Ts...>> {
public:
    using Type = IndexedTuple<TypeList<Ts...>, TypeList<ExpandToType<Ts, TextureBinding>...>>;
};

} // namespace detail

template <class TextureTypeList>
using TextureBindings = typename detail::TextureBindings<TextureTypeList>::Type;

} // namespace gfx
} // namespace mbgl
