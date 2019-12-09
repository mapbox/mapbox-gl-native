#pragma once

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/optional.hpp>

#include <vector>

namespace mbgl {

class Anchor {
public:
    Point<float> point;
    float angle = 0.0f;
    optional<std::size_t> segment;

    Anchor(float x_, float y_, float angle_, optional<std::size_t> segment_ = nullopt)
        : point(x_, y_), angle(angle_), segment(segment_) {}
};

using Anchors = std::vector<Anchor>;

} // namespace mbgl
