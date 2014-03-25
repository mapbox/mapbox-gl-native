#ifndef LLMR_GEOMETRY_INTERPOLATE
#define LLMR_GEOMETRY_INTERPOLATE

#include <llmr/geometry/anchor.hpp>
#include <llmr/util/math.hpp>

namespace llmr {

Anchors interpolate(const std::vector<Coordinate> &vertices, float spacing,
                    float minScale = 0.0f, int start = 0);
}

#endif
