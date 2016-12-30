#pragma once

#include <mbgl/util/util.hpp>
#include <mbgl/util/traits.hpp>

#include <cstdint>

namespace mbgl {

using EnumType = uint32_t;

enum class MapMode : EnumType {
    Continuous, // continually updating map
    Still, // a once-off still image
};

// We can avoid redundant GL calls when it is known that the GL context is not
// being shared. In a shared GL context case, we need to make sure that the
// correct GL configurations are in use - they might have changed between render
// calls.
enum class GLContextMode : EnumType {
    Unique,
    Shared,
};

// We can choose to constrain the map both horizontally or vertically, or only
// vertically e.g. while panning.
enum class ConstrainMode : EnumType {
    None,
    HeightOnly,
    WidthAndHeight,
};

// Satisfies embedding platforms that requires the viewport coordinate systems
// to be set according to its standards.
enum class ViewportMode : EnumType {
    Default,
    FlippedY,
};

enum class MapDebugOptions : EnumType {
    NoDebug     = 0,
    TileBorders = 1 << 1,
    ParseStatus = 1 << 2,
    Timestamps  = 1 << 3,
    Collision   = 1 << 4,
    Overdraw    = 1 << 5,
// FIXME: https://github.com/mapbox/mapbox-gl-native/issues/5117
#if not MBGL_USE_GLES2
    StencilClip = 1 << 6,
    DepthBuffer = 1 << 7,
#endif // MBGL_USE_GLES2
};

MBGL_CONSTEXPR MapDebugOptions operator|(MapDebugOptions lhs, MapDebugOptions rhs) {
    return MapDebugOptions(mbgl::underlying_type(lhs) | mbgl::underlying_type(rhs));
}

MBGL_CONSTEXPR MapDebugOptions& operator|=(MapDebugOptions& lhs, MapDebugOptions rhs) {
    return (lhs = MapDebugOptions(mbgl::underlying_type(lhs) | mbgl::underlying_type(rhs)));
}

MBGL_CONSTEXPR bool operator&(MapDebugOptions lhs, MapDebugOptions rhs) {
    return mbgl::underlying_type(lhs) & mbgl::underlying_type(rhs);
}

MBGL_CONSTEXPR MapDebugOptions& operator&=(MapDebugOptions& lhs, MapDebugOptions rhs) {
    return (lhs = MapDebugOptions(mbgl::underlying_type(lhs) & mbgl::underlying_type(rhs)));
}

MBGL_CONSTEXPR MapDebugOptions operator~(MapDebugOptions value) {
    return MapDebugOptions(~mbgl::underlying_type(value));
}

} // namespace mbgl
