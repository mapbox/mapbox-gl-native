#pragma once

namespace mbgl {
namespace style {

class Undefined {};

inline bool operator==(const Undefined&, const Undefined&) { return true; }
inline bool operator!=(const Undefined&, const Undefined&) { return false; }

} // namespace style
} // namespace mbgl
