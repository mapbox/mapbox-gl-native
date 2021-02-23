#pragma once

#include <mbgl/gfx/types.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/util/type_list.hpp>
#include <mbgl/util/indexed_tuple.hpp>

#include <memory>
#include <cassert>

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
    Texture(const Size size_, std::unique_ptr<TextureResource>&& resource_)
        : size(size_), resource(std::move(resource_)) {
    }

    template <typename T = TextureResource>
    T& getResource() const {
        assert(resource);
        return static_cast<T&>(*resource);
    }

    Size size;

protected:
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

template <class>
class TextureBindings;

template <class... Ts>
class TextureBindings<TypeList<Ts...>> final
    : public IndexedTuple<TypeList<Ts...>, TypeList<ExpandToType<Ts, TextureBinding>...>> {
    using Base = IndexedTuple<TypeList<Ts...>, TypeList<ExpandToType<Ts, TextureBinding>...>>;

public:
    template <class... Args>
    TextureBindings(Args&&... args) : Base(std::forward<Args>(args)...) {
    }
};

} // namespace gfx
} // namespace mbgl
