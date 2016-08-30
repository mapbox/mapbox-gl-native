#pragma once

#include <mbgl/util/constants.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/math/clamp.hpp>

#include <cmath>

namespace mbgl {

class Projection {

public:
    static double getMetersPerPixelAtLatitude(double lat, double zoom) {
        const double constrainedZoom = util::clamp(zoom, util::MIN_ZOOM, util::MAX_ZOOM);
        const double mapPixelWidthAtZoom = std::pow(2.0, constrainedZoom) * util::tileSize;
        const double constrainedLatitude = util::clamp(lat, -util::LATITUDE_MAX, util::LATITUDE_MAX);
        return std::cos(constrainedLatitude * util::DEG2RAD) * util::M2PI * util::EARTH_RADIUS_M / mapPixelWidthAtZoom;
    }

    static ProjectedMeters projectedMetersForLatLng(const LatLng& latLng) {
        const double constrainedLatitude = util::clamp(latLng.latitude, -util::LATITUDE_MAX, util::LATITUDE_MAX);
        const double constrainedLongitude = util::clamp(latLng.longitude, -util::LONGITUDE_MAX, util::LONGITUDE_MAX);

        const double m = 1 - 1e-15;
        const double f = util::clamp(std::sin(util::DEG2RAD * constrainedLatitude), -m, m);

        const double easting  = util::EARTH_RADIUS_M * constrainedLongitude * util::DEG2RAD;
        const double northing = 0.5 * util::EARTH_RADIUS_M * std::log((1 + f) / (1 - f));

        return ProjectedMeters(northing, easting);
    }

    static LatLng latLngForProjectedMeters(const ProjectedMeters& projectedMeters) {
        double latitude = (2 * std::atan(std::exp(projectedMeters.northing / util::EARTH_RADIUS_M)) - (M_PI / 2)) * util::RAD2DEG;
        double longitude = projectedMeters.easting * util::RAD2DEG / util::EARTH_RADIUS_M;

        latitude = util::clamp(latitude, -util::LATITUDE_MAX, util::LATITUDE_MAX);
        longitude = util::clamp(longitude, -util::LONGITUDE_MAX, util::LONGITUDE_MAX);

        return LatLng(latitude, longitude);
    }
};

} // namespace mbgl
