#pragma once

#include <mbgl/gl/object.hpp>

#include <array>

namespace mbgl {
namespace gl {

template <RenderbufferType renderbufferType>
class Renderbuffer {
public:
    using type = std::integral_constant<RenderbufferType, renderbufferType>;
    std::array<uint16_t, 2> size;
    gl::UniqueRenderbuffer renderbuffer;
};

} // namespace gl
} // namespace mbgl
