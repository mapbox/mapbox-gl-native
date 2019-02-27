#pragma once

#include <mbgl/platform/gl_functions.hpp>

namespace mbgl {
namespace gl {

template <typename T>
class Enum {
public:
    static T from(const platform::GLint);
    static platform::GLenum to(T);
};

} // namespace gl
} // namespace mbgl
