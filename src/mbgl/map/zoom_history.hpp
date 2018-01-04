#pragma once

#include <mbgl/util/chrono.hpp>

#include <cmath>

namespace mbgl {

struct ZoomHistory {
    float lastZoom;
    float lastFloorZoom;
    float lastIntegerZoom;
    TimePoint lastIntegerZoomTime;
    bool first = true;

    bool update(float z, const TimePoint& now) {
        constexpr TimePoint zero = TimePoint(Duration::zero());
        const float floorZ = std::floor(z);

        if (first) {
            first = false;
            lastIntegerZoom = floorZ;
            lastIntegerZoomTime = zero;
            lastZoom = z;
            lastFloorZoom = floorZ;
            return true;
        }

        if (lastFloorZoom > floorZ) {
            lastIntegerZoom = floorZ + 1;
            lastIntegerZoomTime = now == Clock::time_point::max() ? zero : now;
        } else if (lastFloorZoom < floorZ) {
            lastIntegerZoom = floorZ;
            lastIntegerZoomTime = now == Clock::time_point::max() ? zero : now;
        }

        if (z != lastZoom) {
            lastZoom = z;
            lastFloorZoom = floorZ;
            return true;
        }

        return false;
    }
};

} // namespace mbgl
