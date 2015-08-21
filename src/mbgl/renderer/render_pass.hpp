#ifndef MBGL_RENDERER_RENDER_PASS
#define MBGL_RENDERER_RENDER_PASS

#include <cstdint>
#include <type_traits>

namespace mbgl {

enum class RenderPass : uint8_t {
    None = 0,
    Opaque = 1 << 0,
    Translucent = 1 << 1,
};

constexpr inline RenderPass operator|(RenderPass a, RenderPass b) {
    return static_cast<RenderPass>(static_cast<std::underlying_type<RenderPass>::type>(a) |
                                   static_cast<std::underlying_type<RenderPass>::type>(b));
}

inline RenderPass operator|=(RenderPass& a, RenderPass b) {
    return (a = a | b);
}

constexpr inline RenderPass operator&(RenderPass a, RenderPass b) {
    return static_cast<RenderPass>(static_cast<std::underlying_type<RenderPass>::type>(a) &
                                   static_cast<std::underlying_type<RenderPass>::type>(b));
}

}

#endif
