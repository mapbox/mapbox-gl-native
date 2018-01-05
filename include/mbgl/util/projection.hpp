#pragma once

#include <mbgl/util/constants.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/math/clamp.hpp>

namespace mbgl {

class ProjectedMeters {
private:
    double _northing; // Distance measured northwards.
    double _easting;  // Distance measured eastwards.

public:
    ProjectedMeters(double n_ = 0, double e_ = 0)
        : _northing(n_), _easting(e_) {
        if (std::isnan(_northing)) {
            throw std::domain_error("northing must not be NaN");
        }
        if (std::isnan(_easting)) {
            throw std::domain_error("easting must not be NaN");
        }
    }

    double northing() const { return _northing; }
    double easting() const { return _easting; }

    friend bool operator==(const ProjectedMeters& a, const ProjectedMeters& b) {
        return a._northing == b._northing && a._easting == b._easting;
    }

    friend bool operator!=(const ProjectedMeters& a, const ProjectedMeters& b) {
        return !(a == b);
    }
};

// Spherical Mercator projection
// http://docs.openlayers.org/library/spherical_mercator.html
class Projection {
public:
    // Map pixel width at given scale.
    static double worldSize(double scale) {
        return scale * util::tileSize;
    }

    static double getMetersPerPixelAtLatitude(double lat, double zoom) {
        const double constrainedZoom = util::clamp(zoom, util::MIN_ZOOM, util::MAX_ZOOM);
        const double constrainedScale = std::pow(2.0, constrainedZoom);
        const double constrainedLatitude = util::clamp(lat, -util::LATITUDE_MAX, util::LATITUDE_MAX);
        return std::cos(constrainedLatitude * util::DEG2RAD) * util::M2PI * util::EARTH_RADIUS_M / worldSize(constrainedScale);
    }

    static ProjectedMeters projectedMetersForLatLng(const LatLng& latLng) {
        const double constrainedLatitude = util::clamp(latLng.latitude(), -util::LATITUDE_MAX, util::LATITUDE_MAX);
        const double constrainedLongitude = util::clamp(latLng.longitude(), -util::LONGITUDE_MAX, util::LONGITUDE_MAX);

        const double m = 1 - 1e-15;
        const double f = util::clamp(std::sin(util::DEG2RAD * constrainedLatitude), -m, m);

        const double easting  = util::EARTH_RADIUS_M * constrainedLongitude * util::DEG2RAD;
        const double northing = 0.5 * util::EARTH_RADIUS_M * std::log((1 + f) / (1 - f));

        return ProjectedMeters(northing, easting);
    }

    static LatLng latLngForProjectedMeters(const ProjectedMeters& projectedMeters) {
        double latitude = (2 * std::atan(std::exp(projectedMeters.northing() / util::EARTH_RADIUS_M)) - (M_PI / 2.0)) * util::RAD2DEG;
        double longitude = projectedMeters.easting() * util::RAD2DEG / util::EARTH_RADIUS_M;

        latitude = util::clamp(latitude, -util::LATITUDE_MAX, util::LATITUDE_MAX);
        longitude = util::clamp(longitude, -util::LONGITUDE_MAX, util::LONGITUDE_MAX);

        return LatLng(latitude, longitude);
    }

    static Point<double> project(const LatLng& latLng, double scale) {
        return project_(latLng, worldSize(scale));
    }

    static Point<double> project(const LatLng& latLng, uint8_t zoom) {
        return project_(latLng, std::pow(2.0, zoom));
    }

    static LatLng unproject(const Point<double>& p, double scale, LatLng::WrapMode wrapMode = LatLng::Unwrapped) {
        auto p2 = p * util::DEGREES_MAX / worldSize(scale);
        return LatLng {
            util::DEGREES_MAX / M_PI * std::atan(std::exp((util::LONGITUDE_MAX - p2.y) * util::DEG2RAD)) - 90.0,
            p2.x - util::LONGITUDE_MAX,
            wrapMode
        };
    }
    
    // Project lat, lon to point in a zoom-dependent world size
    static Point<double> project(const LatLng& point, uint8_t zoom, uint16_t tileSize) {
        const double t2z = tileSize * std::pow(2, zoom);
        Point<double> pt = project_(point, t2z);
        // Flip y coordinate
        auto x = ::round(std::min(pt.x, t2z));
        auto y = ::round(std::min(t2z - pt.y, t2z));
        return { x, y };
    }
private:
    static Point<double> project_(const LatLng& latLng, double worldSize) {
        return Point<double> {
            util::LONGITUDE_MAX + latLng.longitude(),
            util::LONGITUDE_MAX - util::RAD2DEG * std::log(std::tan(M_PI / 4 + latLng.latitude() * M_PI / util::DEGREES_MAX))
        } * worldSize / util::DEGREES_MAX;
    }
};

} // namespace mbgl
