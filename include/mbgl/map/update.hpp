#ifndef MBGL_MAP_UPDATE
#define MBGL_MAP_UPDATE

#include <cstdint>

namespace mbgl {

enum class Update : uint32_t {
    Nothing                   = 0,
    Dimensions                = 1 << 1,
    DefaultTransition         = 1 << 2,
    Classes                   = 1 << 3,
    Zoom                      = 1 << 4,
    RenderStill               = 1 << 5,
    Repaint                   = 1 << 6,
};

inline Update operator| (const Update& lhs, const Update& rhs) {
    return Update(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

inline Update& operator|=(Update& lhs, const Update& rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline bool operator& (const Update& lhs, const Update& rhs) {
    return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs);
}

} // namespace mbgl

#endif // MBGL_MAP_UPDATE
