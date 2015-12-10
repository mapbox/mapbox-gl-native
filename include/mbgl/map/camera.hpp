#ifndef MBGL_MAP_CAMERA
#define MBGL_MAP_CAMERA

#include <mapbox/optional.hpp>

#include <mbgl/util/geo.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/unitbezier.hpp>

#include <functional>

namespace mbgl {

struct CameraOptions {
    mapbox::util::optional<LatLng> center;      // Map center (Degrees)
    mapbox::util::optional<double> zoom;        // Map zoom level Positive Numbers > 0 and < 18
    mapbox::util::optional<double> angle;       // Map rotation bearing in Radians counter-clockwise from north. The value is wrapped to [−π rad, π rad]
    mapbox::util::optional<double> pitch;       // Map angle in degrees at which the camera is looking to ground (Radians)
    mapbox::util::optional<Duration> duration;  // Animation time length (Nanoseconds)
    mapbox::util::optional<double> speed;
    mapbox::util::optional<double> curve;
    mapbox::util::optional<mbgl::util::UnitBezier> easing;
    std::function<void(double)> transitionFrameFn;
    std::function<void()> transitionFinishFn;
};

} // namespace mbgl

#endif /* MBGL_MAP_CAMERA */
