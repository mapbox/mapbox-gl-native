#ifndef MBGL_UTIL_TILE_COVER
#define MBGL_UTIL_TILE_COVER

#include <mbgl/map/tile_id.hpp>
#include <mbgl/util/box.hpp>
#include <mbgl/style/types.hpp>

#include <forward_list>

namespace mbgl {

class TransformState;
class SourceInfo;

int32_t coveringZoomLevel(double zoom,
                          SourceType type,
                          uint16_t tileSize);

std::forward_list<TileID> tileCover(const TransformState&,
                                    SourceType,
                                    uint16_t tileSize,
                                    const SourceInfo&);

} // namespace mbgl

#endif
