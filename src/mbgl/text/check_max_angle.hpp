#ifndef MBGL_TEXT_CHECK_MAX_ANGLE
#define MBGL_TEXT_CHECK_MAX_ANGLE

#include <mbgl/geometry/anchor.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

bool checkMaxAngle(const std::vector<Coordinate> &line, Anchor &anchor, const float labelLength,
        const float windowSize, const float maxAngle);

}

#endif
