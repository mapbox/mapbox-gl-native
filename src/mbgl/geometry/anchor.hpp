#pragma once

#include <mbgl/util/geometry.hpp>

#include <vector>

namespace mbgl {

struct Anchor {
    Point<float> point;
    float angle = 0.0f;
    float scale = 0.0f;
    int segment = -1;

    explicit Anchor(float x_, float y_, float angle_, float scale_)
        : point(x_, y_), angle(angle_), scale(scale_) {}
    explicit Anchor(float x_, float y_, float angle_, float scale_, int segment_)
        : point(x_, y_), angle(angle_), scale(scale_), segment(segment_) {}

};

typedef std::vector<Anchor> Anchors;

} // namespace mbgl
