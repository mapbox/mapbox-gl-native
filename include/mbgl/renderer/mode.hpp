#pragma once

#include <cstdint>

namespace mbgl {

using EnumType = uint32_t;

// We can avoid redundant GL calls when it is known that the GL context is not
// being shared. In a shared GL context case, we need to make sure that the
// correct GL configurations are in use - they might have changed between render
// calls.
enum class GLContextMode : EnumType {
    Unique,
    Shared,
};

} // namespace mbgl
