#pragma once

#include <mbgl/util/bitmask_operations.hpp>

#include <cstdint>

namespace mbgl {

enum class RenderPass : uint8_t {
    None = 0,
    Opaque = 1 << 0,
    Translucent = 1 << 1,
    Pass3D = 1 << 2,
};

// Defines whether the overdraw shaders should be used instead of the regular shaders.
enum class PaintMode : bool {
    Regular = false,
    Overdraw = true,
};

} // namespace mbgl
