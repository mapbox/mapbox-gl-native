#pragma once

#include <mbgl/util/chrono.hpp>

#include <cmath>

namespace mbgl {

struct ZoomHistory {
    float lastZoom;
    float lastIntegerZoom;
    TimePoint lastIntegerZoomTime;
    bool first = true;

    bool update(float z, const TimePoint& now) {
        if (first) {
            first = false;
            lastIntegerZoom = std::floor(z);
            lastIntegerZoomTime = TimePoint(Duration::zero());
            lastZoom = z;
            return true;
        } else {
            if (std::floor(lastZoom) < std::floor(z)) {
                lastIntegerZoom = std::floor(z);
                lastIntegerZoomTime = now;
            } else if (std::floor(lastZoom) > std::floor(z)) {
                lastIntegerZoom = std::floor(z + 1);
                lastIntegerZoomTime = now;
            }

            if (z != lastZoom) {
                lastZoom = z;
                return true;
            }

            return false;
        }
    }
};

} // namespace mbgl
