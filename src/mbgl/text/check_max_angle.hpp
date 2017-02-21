#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {

class Anchor;

bool checkMaxAngle(const GeometryCoordinates& line,
                   const Anchor& anchor,
                   const float labelLength,
                   const float windowSize,
                   const float maxAngle);

} // namespace mbgl
