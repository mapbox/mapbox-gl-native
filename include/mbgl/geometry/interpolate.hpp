#ifndef MBGL_GEOMETRY_INTERPOLATE
#define MBGL_GEOMETRY_INTERPOLATE

#include <mbgl/geometry/anchor.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

Anchors interpolate(const std::vector<Coordinate> &vertices, float spacing,
                    float minScale, float maxScale, float tilePixelRatio, int start = 0);
}

#endif
