#include <mbgl/util/geo.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/math/clamp.hpp>
#include <mbgl/util/tile_range.hpp>

#include <cmath>

namespace mbgl {

namespace {

double lat_(const uint8_t z, const int64_t y) {
    const double n = M_PI - 2.0 * M_PI * y / std::pow(2.0, z);
    return util::RAD2DEG * std::atan(0.5 * (std::exp(n) - std::exp(-n)));
}

double lon_(const uint8_t z, const int64_t x) {
    return x / std::pow(2.0, z) * util::DEGREES_MAX - util::LONGITUDE_MAX;
}

} // end namespace

LatLng::LatLng(const CanonicalTileID& id) : lat(lat_(id.z, id.y)), lon(lon_(id.z, id.x)) {
}

LatLng::LatLng(const UnwrappedTileID& id)
    : lat(lat_(id.canonical.z, id.canonical.y)),
      lon(lon_(id.canonical.z, id.canonical.x) + id.wrap * util::DEGREES_MAX) {
}

LatLngBounds::LatLngBounds(const CanonicalTileID& id)
    : sw({ lat_(id.z, id.y + 1), lon_(id.z, id.x) }),
      ne({ lat_(id.z, id.y), lon_(id.z, id.x + 1) }) {
}

bool LatLngBounds::contains(const CanonicalTileID& tileID) const {
    return util::TileRange::fromLatLngBounds(*this, tileID.z).contains(tileID);
}

bool LatLngBounds::contains(const LatLng& point, LatLng::WrapMode wrap /*= LatLng::Unwrapped*/) const {
    bool containsLatitude = point.latitude() >= sw.latitude() &&
                            point.latitude() <= ne.latitude();
    if (!containsLatitude) {
        return false;
    }

    bool containsUnwrappedLongitude = point.longitude() >= sw.longitude() &&
                                      point.longitude() <= ne.longitude();
    if (containsUnwrappedLongitude) {
        return true;
    } else if (wrap == LatLng::Wrapped) {
        LatLngBounds wrapped(sw.wrapped(), ne.wrapped());
        auto ptLon = point.wrapped().longitude();
        if (crossesAntimeridian()) {
            return (ptLon >= wrapped.sw.longitude() &&
                    ptLon <= util::LONGITUDE_MAX) ||
                   (ptLon <= wrapped.ne.longitude() &&
                    ptLon >= -util::LONGITUDE_MAX);
        } else {
            return (ptLon >= wrapped.sw.longitude() &&
                    ptLon <= wrapped.ne.longitude());
        }
    }
    return false;
}

bool LatLngBounds::contains(const LatLngBounds& area, LatLng::WrapMode wrap /*= LatLng::Unwrapped*/) const {
    bool containsLatitude = area.north() <= north() && area.south() >= south();
    if (!containsLatitude) {
        return false;
    }

    bool containsUnwrapped = area.east() <= east() && area.west() >= west();
    if(containsUnwrapped) {
        return true;
    } else if (wrap == LatLng::Wrapped) {
        LatLngBounds wrapped(sw.wrapped(), ne.wrapped());
        LatLngBounds other(area.sw.wrapped(), area.ne.wrapped());
        if (crossesAntimeridian() & !area.crossesAntimeridian()) {
            return (other.east() <= util::LONGITUDE_MAX && other.west() >= wrapped.west()) ||
                   (other.east() <= wrapped.east() && other.west() >= -util::LONGITUDE_MAX);
        } else {
            return other.east() <= wrapped.east() && other.west() >= wrapped.west();
        }
    }
    return false;
}

bool LatLngBounds::intersects(const LatLngBounds area, LatLng::WrapMode wrap /*= LatLng::Unwrapped*/) const {
    bool latitudeIntersects = area.north() > south() && area.south() < north();
    if (!latitudeIntersects) {
        return false;
    }

    bool longitudeIntersects = area.east() > west() && area.west() < east();
    if (longitudeIntersects) {
        return true;
    } else if (wrap == LatLng::Wrapped) {
        LatLngBounds wrapped(sw.wrapped(), ne.wrapped());
        LatLngBounds other(area.sw.wrapped(), area.ne.wrapped());
        if (crossesAntimeridian()) {
            return area.crossesAntimeridian() ||
                   other.east() > wrapped.west() ||
                   other.west() < wrapped.east();
        } else if (other.crossesAntimeridian()){
            return other.east() > wrapped.west() ||
                   other.west() < wrapped.east();
        } else {
            return other.east() > wrapped.west() &&
                   other.west() < wrapped.east();
        }
    }
    return false;
}

ScreenCoordinate EdgeInsets::getCenter(uint16_t width, uint16_t height) const {
    return {
        (width - left() - right()) / 2.0 + left(),
        (height - top() - bottom()) / 2.0 + top(),
    };
}

} // end namespace mbgl
