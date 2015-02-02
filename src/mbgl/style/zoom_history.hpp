#ifndef MBGL_STYLE_ZOOM_HISTORY
#define MBGL_STYLE_ZOOM_HISTORY

#include <mbgl/util/time.hpp>

#include <cmath>

namespace mbgl {

struct ZoomHistory {
    float lastZoom;
    float lastIntegerZoom;
    timestamp lastIntegerZoomTime;
    bool first = true;

    void update(float z, timestamp now) {
        if (first) {
            first = false;

            lastIntegerZoom = std::floor(z);
            lastIntegerZoomTime = 0;
            lastZoom = z;
        }

        if (std::floor(lastZoom) < std::floor(z)) {
            lastIntegerZoom = std::floor(z);
            lastIntegerZoomTime = now;

        } else if (std::floor(lastZoom) > std::floor(z)) {
            lastIntegerZoom = std::floor(z + 1);
            lastIntegerZoomTime = now;
        }

        lastZoom = z;
    }
};
}

#endif
