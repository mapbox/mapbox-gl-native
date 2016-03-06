#ifndef MBGL_MAP_UPDATE
#define MBGL_MAP_UPDATE

#include <cstdint>

namespace mbgl {

enum class Update : uint8_t {
    Nothing                   = 0,
    Dimensions                = 1 << 1,
    Classes                   = 1 << 2,
    RecalculateStyle          = 1 << 3,
    RenderStill               = 1 << 4,
    Repaint                   = 1 << 5,
    Annotations               = 1 << 6,
};

inline Update operator| (const Update& lhs, const Update& rhs) {
    return Update(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

inline Update& operator|=(Update& lhs, const Update& rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline bool operator& (const Update& lhs, const Update& rhs) {
    return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
}

} // namespace mbgl

#endif // MBGL_MAP_UPDATE
