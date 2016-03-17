#ifndef MBGL_UTIL_GEO
#define MBGL_UTIL_GEO

#include <mbgl/util/math.hpp>
#include <mbgl/util/vec.hpp>
#include <mbgl/util/constants.hpp>

#include <cmath>

namespace mbgl {

class TileID;

using ScreenCoordinate = vec2<double>;

class LatLng {
public:
    double latitude;
    double longitude;

    enum WrapMode : bool { Unwrapped, Wrapped };

    LatLng(double lat = 0, double lon = 0, WrapMode mode = Unwrapped)
        : latitude(lat), longitude(lon) {
        if (std::isnan(lat)) throw std::domain_error("latitude must not be NaN");
        if (std::isnan(lon)) throw std::domain_error("longitude must not be NaN");
        if (!std::isfinite(lat)) throw std::domain_error("latitude must not be infinite");
        if (!std::isfinite(lon)) throw std::domain_error("longitude must not be infinite");
        if (mode == Wrapped) wrap();
    }

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

    // Constructs a LatLng object with the top left position of the specified tile.
    LatLng(const TileID& id);

    ScreenCoordinate project() const;
};

inline bool operator==(const LatLng& a, const LatLng& b) {
    return a.latitude == b.latitude && a.longitude == b.longitude;
}

inline bool operator!=(const LatLng& a, const LatLng& b) {
    return !(a == b);
}

class ProjectedMeters {
public:
    double northing = 0;
    double easting = 0;

    ProjectedMeters(double n, double e)
        : northing(n), easting(e) {}
};

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
    LatLngBounds(const TileID&);

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

    LatLngBounds(const LatLng& sw_, const LatLng& ne_)
        : sw(sw_), ne(ne_) {}

    friend bool operator==(const LatLngBounds&, const LatLngBounds&);
    friend bool operator!=(const LatLngBounds&, const LatLngBounds&);
};

inline bool operator==(const LatLngBounds& a, const LatLngBounds& b) {
    return a.sw == b.sw && a.ne == b.ne;
}

inline bool operator!=(const LatLngBounds& a, const LatLngBounds& b) {
    return !(a == b);
}

class MetersBounds {
public:
    ProjectedMeters sw;
    ProjectedMeters ne;

    MetersBounds(const ProjectedMeters& sw_, const ProjectedMeters& ne_)
        : sw(sw_), ne(ne_) {}
};

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

} // namespace mbgl

#endif
