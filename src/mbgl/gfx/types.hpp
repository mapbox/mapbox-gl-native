#pragma once

#include <cstdint>

namespace mbgl {
namespace gfx {

enum class PrimitiveType : uint8_t {
    Points,
    Lines,
    LineLoop,
    LineStrip,
    Triangles,
    TriangleStrip,
    TriangleFan
};

} // namespace gfx
} // namespace mbgl
