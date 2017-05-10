#pragma once

#include <mbgl/util/geometry.hpp>

#include <vector>

namespace mbgl {

class Anchor {
public:
    Point<float> point;
    float angle = 0.0f;
    float scale = 0.0f;
    int segment = -1;

    Anchor(float x_, float y_, float angle_, float scale_, int segment_ = -1)
        : point(x_, y_), angle(angle_), scale(scale_), segment(segment_) {}
};

using Anchors = std::vector<Anchor>;

} // namespace mbgl
