#pragma once

#include <mbgl/math/wrap.hpp>
#include <mbgl/util/constants.hpp>

#include <mapbox/geometry/point.hpp>
#include <mapbox/geometry/point_arithmetic.hpp>
#include <mapbox/geometry/line_string.hpp>
#include <mapbox/geometry/box.hpp>

#include <cmath>

namespace mbgl {

class CanonicalTileID;
class UnwrappedTileID;

using ScreenCoordinate = mapbox::geometry::point<double>;
using ScreenLineString = mapbox::geometry::line_string<double>;
using ScreenBox        = mapbox::geometry::box<double>;

class LatLng {
public:
    struct null {};

    double latitude;
    double longitude;

    enum WrapMode : bool { Unwrapped, Wrapped };

    LatLng(null) : latitude(std::numeric_limits<double>::quiet_NaN()), longitude(latitude) {}

    LatLng(double lat = 0, double lon = 0, WrapMode mode = Unwrapped)
        : latitude(lat), longitude(lon) { if (mode == Wrapped) wrap(); }

    LatLng wrapped() const { return { latitude, longitude, Wrapped }; }

    void wrap() {
        longitude = util::wrap(longitude, -util::LONGITUDE_MAX, util::LONGITUDE_MAX);
    }

    // If the distance from start to end longitudes is between half and full
    // world, unwrap the start longitude to ensure the shortest path is taken.
    void unwrapForShortestPath(const LatLng& end) {
        const double delta = std::abs(end.longitude - longitude);
        if (delta < util::LONGITUDE_MAX || delta > util::DEGREES_MAX) return;
        if (longitude > 0 && end.longitude < 0) longitude -= util::DEGREES_MAX;
        else if (longitude < 0 && end.longitude > 0) longitude += util::DEGREES_MAX;
    }

    explicit operator bool() const {
        return !(std::isnan(latitude) || std::isnan(longitude));
    }

    // Constructs a LatLng object with the top left position of the specified tile.
    LatLng(const CanonicalTileID& id);
    LatLng(const UnwrappedTileID& id);
};

constexpr bool operator==(const LatLng& a, const LatLng& b) {
    return a.latitude == b.latitude && a.longitude == b.longitude;
}

constexpr bool operator!=(const LatLng& a, const LatLng& b) {
    return !(a == b);
}

class ProjectedMeters {
public:
    double northing;
    double easting;

    ProjectedMeters(double n = 0, double e = 0)
        : northing(n), easting(e) {}

    explicit operator bool() const {
        return !(std::isnan(northing) || std::isnan(easting));
    }
};

constexpr bool operator==(const ProjectedMeters& a, const ProjectedMeters& b) {
    return a.northing == b.northing && a.easting == b.easting;
}

class LatLngBounds {
public:
    // Return a bounds covering the entire (unwrapped) world.
    static LatLngBounds world() {
        return LatLngBounds({-90, -180}, {90, 180});
    }

    // Return the bounds consisting of the single point.
    static LatLngBounds singleton(const LatLng& a) {
        return LatLngBounds(a, a);
    }

    // Return the convex hull of two points; the smallest bounds that contains both.
    static LatLngBounds hull(const LatLng& a, const LatLng& b) {
        LatLngBounds bounds(a, a);
        bounds.extend(b);
        return bounds;
    }

    // Return a bounds that may serve as the identity element for the extend operation.
    static LatLngBounds empty() {
        LatLngBounds bounds = world();
        std::swap(bounds.sw, bounds.ne);
        return bounds;
    }

    // Constructs a LatLngBounds object with the tile's exact boundaries.
    LatLngBounds(const CanonicalTileID&);

    double south() const { return sw.latitude; }
    double west()  const { return sw.longitude; }
    double north() const { return ne.latitude; }
    double east()  const { return ne.longitude; }

    LatLng southwest() const { return sw; }
    LatLng northeast() const { return ne; }
    LatLng southeast() const { return LatLng(south(), east()); }
    LatLng northwest() const { return LatLng(north(), west()); }

    LatLng center() const {
        return LatLng((sw.latitude + ne.latitude) / 2,
                      (sw.longitude + ne.longitude) / 2);
    }

    void extend(const LatLng& point) {
        if (point.latitude < sw.latitude) sw.latitude = point.latitude;
        if (point.latitude > ne.latitude) ne.latitude = point.latitude;
        if (point.longitude < sw.longitude) sw.longitude = point.longitude;
        if (point.longitude > ne.longitude) ne.longitude = point.longitude;
    }

    void extend(const LatLngBounds& bounds) {
        extend(bounds.sw);
        extend(bounds.ne);
    }

    bool isEmpty() const {
        return sw.latitude > ne.latitude ||
               sw.longitude > ne.longitude;
    }

    bool contains(const LatLng& point) const {
        return (point.latitude  >= sw.latitude  &&
                point.latitude  <= ne.latitude  &&
                point.longitude >= sw.longitude &&
                point.longitude <= ne.longitude);
    }

    bool intersects(const LatLngBounds area) const {
        return (area.ne.latitude  > sw.latitude  &&
                area.sw.latitude  < ne.latitude  &&
                area.ne.longitude > sw.longitude &&
                area.sw.longitude < ne.longitude);
    }

private:
    LatLng sw;
    LatLng ne;

    LatLngBounds(LatLng sw_, LatLng ne_)
        : sw(std::move(sw_)), ne(std::move(ne_)) {}

    friend constexpr bool operator==(const LatLngBounds&, const LatLngBounds&);
    friend constexpr bool operator!=(const LatLngBounds&, const LatLngBounds&);
};

constexpr bool operator==(const LatLngBounds& a, const LatLngBounds& b) {
    return a.sw == b.sw && a.ne == b.ne;
}

constexpr bool operator!=(const LatLngBounds& a, const LatLngBounds& b) {
    return !(a == b);
}

// Determines the orientation of the map.
enum class NorthOrientation : uint8_t {
    Upwards, // Default
    Rightwards,
    Downwards,
    Leftwards,
};

/// The distance on each side between a rectangle and a rectangle within.
class EdgeInsets {
public:
    double top = 0;     ///< Number of pixels inset from the top edge.
    double left = 0;    ///< Number of pixels inset from the left edge.
    double bottom = 0;  ///< Number of pixels inset from the bottom edge.
    double right = 0;   ///< Number of pixels inset from the right edge.

    EdgeInsets() {}

    EdgeInsets(const double t, const double l, const double b, const double r)
        : top(t), left(l), bottom(b), right(r) {}

    explicit operator bool() const {
        return !(std::isnan(top) || std::isnan(left) || std::isnan(bottom) || std::isnan(right))
            && (top || left || bottom || right);
    }

    void operator+=(const EdgeInsets& o) {
        top += o.top;
        left += o.left;
        bottom += o.bottom;
        right += o.right;
    }

    EdgeInsets operator+(const EdgeInsets& o) const {
        return {
            top + o.top, left + o.left, bottom + o.bottom, right + o.right,
        };
    }

    ScreenCoordinate getCenter(uint16_t width, uint16_t height) const;
};

} // namespace mbgl
