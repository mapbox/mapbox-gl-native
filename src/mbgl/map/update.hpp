#pragma once

#include <mbgl/util/traits.hpp>
#include <mbgl/util/util.hpp>

namespace mbgl {

enum class Update {
    Nothing                   = 0,
    Repaint                   = 1 << 0,
    Classes                   = 1 << 2,
    RecalculateStyle          = 1 << 3,
    AnnotationStyle           = 1 << 6,
    AnnotationData            = 1 << 7,
    Layout                    = 1 << 8
};

MBGL_CONSTEXPR Update operator|(Update lhs, Update rhs) {
    return Update(mbgl::underlying_type(lhs) | mbgl::underlying_type(rhs));
}

MBGL_CONSTEXPR Update& operator|=(Update& lhs, const Update& rhs) {
    return (lhs = lhs | rhs);
}

MBGL_CONSTEXPR bool operator& (Update lhs, Update rhs) {
    return mbgl::underlying_type(lhs) & mbgl::underlying_type(rhs);
}

} // namespace mbgl
