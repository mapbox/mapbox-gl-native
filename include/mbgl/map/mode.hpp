#ifndef MBGL_MAP_MODE
#define MBGL_MAP_MODE

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
    HeightOnly,
    WidthAndHeight,
};

enum class MapDebugOptions : EnumType {
    NoDebug     = 0,
    TileBorders = 1 << 1,
    ParseStatus = 1 << 2,
    Timestamps  = 1 << 3,
    Collision   = 1 << 4,
};

inline MapDebugOptions operator| (const MapDebugOptions& lhs, const MapDebugOptions& rhs) {
    return MapDebugOptions(static_cast<EnumType>(lhs) | static_cast<EnumType>(rhs));
}

inline MapDebugOptions& operator|=(MapDebugOptions& lhs, const MapDebugOptions& rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline bool operator& (const MapDebugOptions& lhs, const MapDebugOptions& rhs) {
    return static_cast<EnumType>(lhs) & static_cast<EnumType>(rhs);
}

} // namespace mbgl

#endif // MBGL_MAP_MODE
