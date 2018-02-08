#pragma once

#include <mbgl/util/util.hpp>
#include <mbgl/util/traits.hpp>

namespace mbgl {

using EnumType = uint32_t;

enum class GLContextMode : EnumType {
    Automatic = 0,

    // We can avoid redundant GL calls when it is known that the GL context is not being shared. In
    // a shared GL context case, we need to make sure that the correct GL configurations are in use 
    // - they might have changed between render calls. If you're issuing other OpenGL calls in the
    // same context (excluding uses of custom layers), then initialize the context with this flag set.
    SharedState                     = 0b00000001,

    // Vertex Array Objects bundle buffer state for faster binding and is available on most modern
    // platforms. However, some older GPUs have bugs with this, and some devices don't support it
    // altogether. Setting this flag allows testing without Vertex Array Objects.
    DisableVAOExtension             = 0b00000010,

    // Some platforms support exporting compiled shader programs as binaries. This allows us to
    // store them and reload the precompiled binary on subsequent loads, which typically speeds
    // up initialization. However, some GPUs have trouble reloading binaries due driver bugs.
    // You can explicitly disable program binaries by setting this flag.
    DisableProgramBinariesExtension = 000000100,
};

MBGL_CONSTEXPR GLContextMode operator|(GLContextMode lhs, GLContextMode rhs) {
    return GLContextMode(mbgl::underlying_type(lhs) | mbgl::underlying_type(rhs));
}

MBGL_CONSTEXPR GLContextMode& operator|=(GLContextMode& lhs, GLContextMode rhs) {
    return (lhs = GLContextMode(mbgl::underlying_type(lhs) | mbgl::underlying_type(rhs)));
}

MBGL_CONSTEXPR bool operator&(GLContextMode lhs, GLContextMode rhs) {
    return mbgl::underlying_type(lhs) & mbgl::underlying_type(rhs);
}

MBGL_CONSTEXPR GLContextMode& operator&=(GLContextMode& lhs, GLContextMode rhs) {
    return (lhs = GLContextMode(mbgl::underlying_type(lhs) & mbgl::underlying_type(rhs)));
}

MBGL_CONSTEXPR GLContextMode operator~(GLContextMode value) {
    return GLContextMode(~mbgl::underlying_type(value));
}

} // namespace mbgl
