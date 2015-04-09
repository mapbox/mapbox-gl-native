#ifndef MBGL_UTIL_TILE_COVER
#define MBGL_UTIL_TILE_COVER

#include <mbgl/map/tile_id.hpp>
#include <mbgl/util/box.hpp>

#include <forward_list>

namespace mbgl {

std::forward_list<TileID> tileCover(int8_t z, const box& bounds);

}

#endif
