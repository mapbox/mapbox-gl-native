#pragma once

#include <mbgl/util/traits.hpp>

namespace mbgl {

enum class Update {
    Nothing                   = 0,
    Repaint                   = 1 << 0
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
