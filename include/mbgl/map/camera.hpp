#ifndef MBGL_MAP_CAMERA
#define MBGL_MAP_CAMERA

#include <mapbox/optional.hpp>

#include <mbgl/util/geo.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/unitbezier.hpp>

namespace mbgl {

struct CameraOptions {
    mapbox::util::optional<LatLng> center;
    mapbox::util::optional<double> zoom;
    mapbox::util::optional<double> angle;
    mapbox::util::optional<double> pitch;
    mapbox::util::optional<Duration> duration;
    mapbox::util::optional<mbgl::util::UnitBezier> easing;
};

}

#endif /* MBGL_MAP_CAMERA */
