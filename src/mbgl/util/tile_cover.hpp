#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/tile_coordinate.hpp>

#include <vector>

namespace mbgl {

class TransformState;
class LatLngBounds;

namespace util {

int32_t coveringZoomLevel(double z, style::SourceType type, uint16_t tileSize);

std::vector<UnwrappedTileID> tileCover(const TransformState&, int32_t z);
std::vector<UnwrappedTileID> tileCover(const LatLngBounds&, int32_t z);

// Compute only the count of tiles needed for tileCover
uint64_t tileCount(const LatLngBounds&, uint8_t z, uint16_t tileSize);

} // namespace util
} // namespace mbgl
