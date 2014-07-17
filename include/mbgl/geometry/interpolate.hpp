#ifndef MBGL_GEOMETRY_INTERPOLATE
#define MBGL_GEOMETRY_INTERPOLATE

#include <mbgl/geometry/anchor.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

Anchors interpolate(const std::vector<Coordinate> &vertices, float spacing,
                    float minScale = 0.0f, int start = 0);
}

#endif
