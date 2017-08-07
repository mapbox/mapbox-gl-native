#include <mbgl/util/geo.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/tile/tile_id.hpp>

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

ScreenCoordinate EdgeInsets::getCenter(uint16_t width, uint16_t height) const {
    return {
        (width - left() - right()) / 2.0 + left(),
        (height - top() - bottom()) / 2.0 + top(),
    };
}

} // end namespace mbgl
