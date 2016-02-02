#ifndef MBGL_UTIL_GEO
#define MBGL_UTIL_GEO

#include <mbgl/util/vec.hpp>

#include <cmath>

namespace mbgl {

class TileID;

using PrecisionPoint = vec2<double>;

class LatLng {
public:
    double latitude = 0;
    double longitude = 0;

    LatLng(double lat = 0, double lon = 0)
        : latitude(lat), longitude(lon) {}

    operator bool() const {
        return !(std::isnan(latitude) || std::isnan(longitude));
    }

    // Constructs a LatLng object with the top left position of the specified tile.
    LatLng(const TileID& id);

    PrecisionPoint project() const;
};

class ProjectedMeters {
public:
    double northing = 0;
    double easting = 0;

    ProjectedMeters(double n = 0, double e = 0)
        : northing(n), easting(e) {}

    operator bool() const {
        return !(std::isnan(northing) || std::isnan(easting));
    }
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
};

class MetersBounds {
public:
    ProjectedMeters sw;
    ProjectedMeters ne;

    MetersBounds(const ProjectedMeters& sw_, const ProjectedMeters& ne_)
        : sw(sw_), ne(ne_) {}

    operator bool() const {
        return sw && ne;
    }
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
    
    EdgeInsets() {}
    
    EdgeInsets(const double t, const double l, const double b, const double r)
        : top(t), left(l), bottom(b), right(r) {}
    
    operator bool() const {
        return top || left || bottom || right;
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
    
    PrecisionPoint getCenter(uint16_t width, uint16_t height) const;
};

} // namespace mbgl

#endif
