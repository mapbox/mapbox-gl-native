#pragma once

#include <mbgl/util/traits.hpp>
#include <mbgl/util/util.hpp>

#include <cstdint>

namespace mbgl {

enum class RenderPass : uint8_t {
    None = 0,
    Opaque = 1 << 0,
    Translucent = 1 << 1,
    Pass3D = 1 << 2,
};

MBGL_CONSTEXPR RenderPass operator|(RenderPass a, RenderPass b) {
    return RenderPass(mbgl::underlying_type(a) | mbgl::underlying_type(b));
}

MBGL_CONSTEXPR RenderPass& operator|=(RenderPass& a, RenderPass b) {
    return (a = a | b);
}

MBGL_CONSTEXPR RenderPass operator&(RenderPass a, RenderPass b) {
    return RenderPass(mbgl::underlying_type(a) & mbgl::underlying_type(b));
}

// Defines whether the overdraw shaders should be used instead of the regular shaders.
enum class PaintMode : bool {
    Regular = false,
    Overdraw = true,
};

} // namespace mbgl
