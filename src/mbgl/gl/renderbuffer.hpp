#pragma once

#include <mbgl/gl/object.hpp>
#include <mbgl/util/size.hpp>

namespace mbgl {
namespace gl {

template <RenderbufferType renderbufferType>
class Renderbuffer {
public:
    using type = std::integral_constant<RenderbufferType, renderbufferType>;
    Size size;
    gl::UniqueRenderbuffer renderbuffer;
    bool dirty = false;
};

} // namespace gl
} // namespace mbgl
