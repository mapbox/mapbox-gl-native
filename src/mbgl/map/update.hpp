#pragma once

#include <mbgl/util/traits.hpp>

namespace mbgl {

enum class Update {
    Nothing                   = 0,
    Classes                   = 1 << 2,
    RecalculateStyle          = 1 << 3,
    RenderStill               = 1 << 4,
    Repaint                   = 1 << 5,
    AnnotationStyle           = 1 << 6,
    AnnotationData            = 1 << 7,
    Layout                    = 1 << 8
};

constexpr Update operator|(Update lhs, Update rhs) {
    return Update(mbgl::underlying_type(lhs) | mbgl::underlying_type(rhs));
}

constexpr Update& operator|=(Update& lhs, const Update& rhs) {
    return (lhs = lhs | rhs);
}

constexpr bool operator& (Update lhs, Update rhs) {
    return mbgl::underlying_type(lhs) & mbgl::underlying_type(rhs);
}

} // namespace mbgl
