#pragma once

#include <mbgl/gl/object.hpp>
#include <mbgl/util/size.hpp>

namespace mbgl {
namespace gl {

template <RenderbufferType renderbufferType>
class Renderbuffer {
public:
    Renderbuffer(Size size_, UniqueRenderbuffer renderbuffer_, bool dirty_ = false)
        : size(std::move(size_)), renderbuffer(std::move(renderbuffer_)), dirty(dirty_) {
    }

    using type = std::integral_constant<RenderbufferType, renderbufferType>;
    Size size;
    UniqueRenderbuffer renderbuffer;

    void shouldClear(bool clear) {
        dirty = clear;
    }
    bool needsClearing() {
        return dirty;
    }

private:
    bool dirty;
};

} // namespace gl
} // namespace mbgl
