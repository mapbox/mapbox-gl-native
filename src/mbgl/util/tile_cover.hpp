#ifndef MBGL_UTIL_TILE_COVER
#define MBGL_UTIL_TILE_COVER

#include <mbgl/map/tile_id.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/tile_coordinate.hpp>

#include <vector>

namespace mbgl {

class TransformState;
class LatLngBounds;

int32_t coveringZoomLevel(double z, SourceType type, uint16_t tileSize);

std::vector<TileID> tileCover(const TransformState&, int32_t z, int32_t actualZ);
std::vector<TileID> tileCover(const LatLngBounds&,   int32_t z, int32_t actualZ);

} // namespace mbgl

#endif
