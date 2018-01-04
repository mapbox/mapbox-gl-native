#pragma once

#include <mbgl/tile/tile_id.hpp>

#include <set>

namespace mbgl {

// A TileMask is a set of TileIDs that describe what part of a tile should be rendered. It omits
// those parts of the tile that are covered by other/better tiles. If the entire tile should be
// rendered, it contains the { 0, 0, 0 } tile. If it's empty, no part of the tile will be rendered.
// TileMasks are typically generated with algorithm::updateTileMasks().
using TileMask = std::set<CanonicalTileID>;

} // namespace mbgl
