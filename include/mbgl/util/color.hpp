#pragma once

#include <array>

namespace mbgl {

// Stores a premultiplied color, with all four channels ranging from 0..1
using Color = std::array<float, 4>;

} // namespace mbgl
