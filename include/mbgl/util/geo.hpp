#pragma once

#include <mbgl/math/clamp.hpp>
#include <mbgl/math/wrap.hpp>
#include <mbgl/util/constants.hpp>

#include <mapbox/geometry/point.hpp>
#include <mapbox/geometry/point_arithmetic.hpp>
#include <mapbox/geometry/line_string.hpp>
#include <mapbox/geometry/box.hpp>

#include <cmath>
#include <stdexcept>

namespace mbgl {

class CanonicalTileID;
class UnwrappedTileID;

using ScreenCoordinate = mapbox::geometry::point<double>;
using ScreenLineString = mapbox::geometry::line_string<double>;
using ScreenBox        = mapbox::geometry::box<double>;

class LatLng {
private:
    double lat;
    double lon;

public:
    enum WrapMode : bool { Unwrapped, Wrapped };

    LatLng(double lat_ = 0, double lon_ = 0, WrapMode mode = Unwrapped)
        : lat(lat_), lon(lon_) {
        if (std::isnan(lat)) {
            throw std::domain_error("latitude must not be NaN");
        }
        if (std::isnan(lon)) {
            throw std::domain_error("longitude must not be NaN");
        }
        if (std::abs(lat) > 90.0) {
            throw std::domain_error("latitude must be between -90 and 90");
        }
        if (!std::isfinite(lon)) {
            throw std::domain_error("longitude must not be infinite");
        }
        if (mode == Wrapped) {
            wrap();
        }
    }

    double latitude() const { return lat; }
    double longitude() const { return lon; }

    LatLng wrapped() const { return { lat, lon, Wrapped }; }

    void wrap() {
        lon = util::wrap(lon, -util::LONGITUDE_MAX, util::LONGITUDE_MAX);
    }

    // If the distance from start to end longitudes is between half and full
    // world, unwrap the start longitude to ensure the shortest path is taken.
    void unwrapForShortestPath(const LatLng& end) {
        const double delta = std::abs(end.lon - lon);
        if (delta < util::LONGITUDE_MAX || delta > util::DEGREES_MAX) return;
        if (lon > 0 && end.lon < 0) lon -= util::DEGREES_MAX;
        else if (lon < 0 && end.lon > 0) lon += util::DEGREES_MAX;
    }

    // Constructs a LatLng object with the top left position of the specified tile.
    LatLng(const CanonicalTileID& id);
    LatLng(const UnwrappedTileID& id);

    friend constexpr bool operator==(const LatLng& a, const LatLng& b) {
        return a.lat == b.lat && a.lon == b.lon;
    }

    friend constexpr bool operator!=(const LatLng& a, const LatLng& b) {
        return !(a == b);
    }
};

class ProjectedMeters {
public:
    double northing;
    double easting;

    ProjectedMeters(double n = 0, double e = 0)
        : northing(n), easting(e) {}
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

    bool valid() const {
        return (sw.latitude() <= ne.latitude()) && (sw.longitude() <= ne.longitude());
    }

    double south() const { return sw.latitude(); }
    double west()  const { return sw.longitude(); }
    double north() const { return ne.latitude(); }
    double east()  const { return ne.longitude(); }

    LatLng southwest() const { return sw; }
    LatLng northeast() const { return ne; }
    LatLng southeast() const { return LatLng(south(), east()); }
    LatLng northwest() const { return LatLng(north(), west()); }

    LatLng center() const {
        return LatLng((sw.latitude() + ne.latitude()) / 2,
                      (sw.longitude() + ne.longitude()) / 2);
    }

    LatLng constrain(const LatLng& p) const {
        if (contains(p)) {
            return p;
        }
        return LatLng {
            util::clamp(p.latitude(), sw.latitude(), ne.latitude()),
            util::clamp(p.longitude(), sw.longitude(), ne.longitude())
        };
    }

    void extend(const LatLng& point) {
        sw = LatLng(std::min(point.latitude(), sw.latitude()),
                    std::min(point.longitude(), sw.longitude()));
        ne = LatLng(std::max(point.latitude(), ne.latitude()),
                    std::max(point.longitude(), ne.longitude()));
    }

    void extend(const LatLngBounds& bounds) {
        extend(bounds.sw);
        extend(bounds.ne);
    }

    bool isEmpty() const {
        return sw.latitude() > ne.latitude() ||
               sw.longitude() > ne.longitude();
    }

    bool contains(const LatLng& point) const {
        return (point.latitude()  >= sw.latitude()  &&
                point.latitude()  <= ne.latitude()  &&
                point.longitude() >= sw.longitude() &&
                point.longitude() <= ne.longitude());
    }

    bool intersects(const LatLngBounds area) const {
        return (area.ne.latitude()  > sw.latitude()  &&
                area.sw.latitude()  < ne.latitude()  &&
                area.ne.longitude() > sw.longitude() &&
                area.sw.longitude() < ne.longitude());
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
    double top = 0;    // Number of pixels inset from the top edge.
    double left = 0;   // Number of pixels inset from the left edge.
    double bottom = 0; // Number of pixels inset from the bottom edge.
    double right = 0;  // Number of pixels inset from the right edge.

    EdgeInsets() {}

    EdgeInsets(const double t, const double l, const double b, const double r)
        : top(t), left(l), bottom(b), right(r) {}


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

constexpr bool operator==(const EdgeInsets& a, const EdgeInsets& b) {
    return a.top == b.top && a.left == b.left && a.bottom == b.bottom && a.right == b.right;
}

constexpr bool operator!=(const EdgeInsets& a, const EdgeInsets& b) {
    return !(a == b);
}

} // namespace mbgl
