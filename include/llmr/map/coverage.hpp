#ifndef LLMR_MAP_COVERAGE
#define LLMR_MAP_COVERAGE

#include <llmr/map/tile.hpp>

#include <forward_list>

namespace llmr {

std::forward_list<Tile::ID> covering_tiles(int32_t zoom, const box& points, const bool use_raster = false, const bool use_retina = false);

}

#endif
