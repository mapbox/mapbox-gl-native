#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/tile_coordinate.hpp>

#include <vector>

namespace mbgl {

class TransformState;
class LatLngBounds;

namespace util {

int32_t coveringZoomLevel(double z, SourceType type, uint16_t tileSize);

std::vector<UnwrappedTileID> tileCover(const TransformState&, uint8_t zoom);
std::vector<UnwrappedTileID> tileCover(const LatLngBounds&, uint8_t zoom);

} // namespace util
} // namespace mbgl
