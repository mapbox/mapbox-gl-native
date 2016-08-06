#pragma once

#include <mbgl/util/traits.hpp>

#include <cstdint>

namespace mbgl {

enum class RenderPass : uint8_t {
    None = 0,
    Opaque = 1 << 0,
    Translucent = 1 << 1,
};

constexpr RenderPass operator|(RenderPass a, RenderPass b) {
    return RenderPass(mbgl::underlying_type(a) | mbgl::underlying_type(b));
}

constexpr RenderPass& operator|=(RenderPass& a, RenderPass b) {
    return (a = a | b);
}

constexpr RenderPass operator&(RenderPass a, RenderPass b) {
    return RenderPass(mbgl::underlying_type(a) & mbgl::underlying_type(b));
}

} // namespace mbgl
