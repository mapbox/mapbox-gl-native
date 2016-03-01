#include <mbgl/util/geo.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/map/tile_id.hpp>

#include <cmath>

namespace mbgl {

LatLng::LatLng(const TileID& id) {
    longitude = id.x / std::pow(2.0, id.z) * util::DEGREES_MAX - util::LONGITUDE_MAX;
    const double n = M_PI - 2.0 * M_PI * id.y / std::pow(2.0, id.z);
    latitude = util::RAD2DEG * std::atan(0.5 * (std::exp(n) - std::exp(-n)));
}

ScreenCoordinate LatLng::project() const {
    // Clamp to the latitude limits of Mercator.
    const double constrainedLatitude = ::fmin(::fmax(latitude, -util::LATITUDE_MAX), util::LATITUDE_MAX);

    // Project a coordinate into unit space in a square map.
    const double sine = std::sin(constrainedLatitude * util::DEG2RAD);
    const double x = longitude / util::DEGREES_MAX + 0.5;
    const double y = 0.5 - 0.25 * std::log((1.0 + sine) / (1.0 - sine)) / M_PI;
    return { x, y };
}

LatLngBounds::LatLngBounds(const TileID& id)
    : sw(TileID{ id.z, id.x, id.y + 1, id.sourceZ }),
      ne(TileID{ id.z, id.x + 1, id.y, id.sourceZ }) {
}

ScreenCoordinate EdgeInsets::getCenter(uint16_t width, uint16_t height) const {
    return {
        (width - left - right) / 2.0f + left,
        (height - top - bottom) / 2.0f + top,
    };
}

} // end namespace mbgl
