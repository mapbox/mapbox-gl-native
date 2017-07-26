#pragma once

#include <cstdint>

namespace mbgl {

enum class ResourceKind : uint8_t {
    Unknown = 0,
    Style = 1,
    Source = 2,
    Tile = 3,
    Glyphs = 4,
    SpriteImage = 5,
    SpriteJSON = 6,
    Image = 7,
};

} // namespace mbgl
