#pragma once

#include <mbgl/util/traits.hpp>

#include <cstdint>

namespace mbgl {

enum class RenderPass : uint8_t {
    None = 0,
    Opaque = 1 << 0,
    Translucent = 1 << 1,
};

constexpr inline RenderPass operator|(RenderPass a, RenderPass b) {
    return static_cast<RenderPass>(mbgl::underlying_type(a) | mbgl::underlying_type(b));
}

inline RenderPass operator|=(RenderPass& a, RenderPass b) {
    return (a = a | b);
}

constexpr inline RenderPass operator&(RenderPass a, RenderPass b) {
    return static_cast<RenderPass>(mbgl::underlying_type(a) & mbgl::underlying_type(b));
}

} // namespace mbgl
