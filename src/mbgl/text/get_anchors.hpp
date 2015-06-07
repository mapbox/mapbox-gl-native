#ifndef MBGL_TEXT_GETANCHORS
#define MBGL_TEXT_GETANCHORS

#include <mbgl/geometry/anchor.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

Anchors getAnchors(const std::vector<Coordinate> &line, float spacing,
        const bool maxAngle, const float left, const float right,
        const float glyphSize, const float boxScale, const float overscaling);
}

#endif
