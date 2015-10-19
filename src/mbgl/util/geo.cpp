#include <mbgl/util/geo.hpp>
#include <mbgl/map/tile_id.hpp>

#include <cmath>

namespace mbgl {

LatLng::LatLng(const TileID& id) {
    longitude = id.x / std::pow(2.0, id.z) * 360.0 - 180;
    const double n = M_PI - 2.0 * M_PI * id.y / std::pow(2.0, id.z);
    latitude = 180.0 / M_PI * std::atan(0.5 * (std::exp(n) - std::exp(-n)));
}

LatLngBounds::LatLngBounds(const TileID& id)
    : sw(TileID{ id.z, id.x, id.y + 1, id.sourceZ }),
      ne(TileID{ id.z, id.x + 1, id.y, id.sourceZ }) {
}

} // end namespace mbgl
