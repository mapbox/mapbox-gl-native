#pragma once

#include <mbgl/platform/gl_functions.hpp>

#include <type_traits>

namespace mbgl {
namespace gl {

template <typename T>
class Enum {
public:
    using InType = std::conditional_t<std::is_same<std::underlying_type_t<T>, bool>::value, platform::GLboolean, platform::GLint>;
    using OutType = std::conditional_t<std::is_same<std::underlying_type_t<T>, bool>::value, platform::GLboolean, platform::GLenum>;

    static T from(const InType);
    static OutType to(T);
};

} // namespace gl
} // namespace mbgl
