#ifndef MBGL_UTIL_PROJECTION
#define MBGL_UTIL_PROJECTION

#include <mbgl/util/constants.hpp>
#include <mbgl/util/geo.hpp>

#include <cmath>

namespace mbgl {

class Projection {

public:
    static inline double getMetersPerPixelAtLatitude(double lat, double zoom) {
        const double mapPixelWidthAtZoom = std::pow(2.0, zoom) * util::tileSize;
        const double constrainedLatitude = ::fmin(::fmax(lat, -util::LATITUDE_MAX), util::LATITUDE_MAX);

        return std::cos(constrainedLatitude * util::DEG2RAD) * util::M2PI * util::EARTH_RADIUS_M / mapPixelWidthAtZoom;
    }

    static inline ProjectedMeters projectedMetersForLatLng(const LatLng& latLng) {
        const double constrainedLatitude = ::fmin(::fmax(latLng.latitude, -util::LATITUDE_MAX), util::LATITUDE_MAX);

        const double m = 1 - 1e-15;
        const double f = ::fmin(::fmax(std::sin(util::DEG2RAD * constrainedLatitude), -m), m);

        const double easting  = util::EARTH_RADIUS_M * latLng.longitude * util::DEG2RAD;
        const double northing = 0.5 * util::EARTH_RADIUS_M * std::log((1 + f) / (1 - f));

        return ProjectedMeters(northing, easting);
    }

    static inline LatLng latLngForProjectedMeters(const ProjectedMeters& projectedMeters) {
        double latitude = (2 * std::atan(std::exp(projectedMeters.northing / util::EARTH_RADIUS_M)) - (M_PI / 2)) * util::RAD2DEG;
        double longitude = projectedMeters.easting * util::RAD2DEG / util::EARTH_RADIUS_M;

        latitude = ::fmin(::fmax(latitude, -util::LATITUDE_MAX), util::LATITUDE_MAX);

        return LatLng(latitude, longitude);
    }
};

} // namespace mbgl

#endif
