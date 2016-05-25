#pragma once

#include <mbgl/util/geo.hpp>

#include <string>

namespace mbgl {

class PointAnnotation {
public:
    PointAnnotation(const LatLng& position_, const std::string& icon_ = "")
        : position(position_.wrapped()), icon(icon_) {}

    const LatLng position;
    const std::string icon;
};

} // namespace mbgl
