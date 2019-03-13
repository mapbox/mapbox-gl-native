#pragma once

#include <mbgl/gfx/types.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/util/type_list.hpp>
#include <mbgl/util/indexed_tuple.hpp>

#include <memory>

#define MBGL_DEFINE_TEXTURE(name_)                                                                 \
    struct name_ {                                                                                 \
        using Value = ::mbgl::gfx::TextureBinding;                                                 \
        static constexpr auto name() {                                                             \
            return #name_;                                                                         \
        }                                                                                          \
    }

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
    Texture(Size size_, std::unique_ptr<TextureResource>&& resource_)
        : size(std::move(size_)), resource(std::move(resource_)) {
    }

    Size size;
    std::unique_ptr<TextureResource> resource;
};

class TextureBinding {
public:
    TextureBinding(TextureResource& resource_,
                   TextureFilterType filter_ = TextureFilterType::Nearest,
                   TextureMipMapType mipmap_ = TextureMipMapType::No,
                   TextureWrapType wrapX_ = TextureWrapType::Clamp,
                   TextureWrapType wrapY_ = TextureWrapType::Clamp)
        : resource(&resource_), filter(filter_), mipmap(mipmap_), wrapX(wrapX_), wrapY(wrapY_) {
    }

    TextureResource* resource;
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
