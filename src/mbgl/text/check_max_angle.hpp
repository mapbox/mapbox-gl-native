#ifndef MBGL_TEXT_CHECK_MAX_ANGLE
#define MBGL_TEXT_CHECK_MAX_ANGLE

#include <mbgl/tile/geometry_tile.hpp>

namespace mbgl {

struct Anchor;

bool checkMaxAngle(const GeometryCoordinates &line, Anchor &anchor, const float labelLength,
        const float windowSize, const float maxAngle);

} // namespace mbgl

#endif
