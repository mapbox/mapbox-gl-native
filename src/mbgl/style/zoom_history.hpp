#ifndef MBGL_STYLE_ZOOM_HISTORY
#define MBGL_STYLE_ZOOM_HISTORY

#include <chrono>
#include <cmath>

namespace mbgl {

struct ZoomHistory {
    float lastZoom;
    float lastIntegerZoom;
    std::chrono::steady_clock::time_point lastIntegerZoomTime;
    bool first = true;

    void update(float z, std::chrono::steady_clock::time_point now) {
        if (first) {
            first = false;

            lastIntegerZoom = std::floor(z);
            lastIntegerZoomTime = std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration(0));
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
