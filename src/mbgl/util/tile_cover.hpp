#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/optional.hpp>

#include <vector>
#include <memory>

namespace mbgl {

class TransformState;
class LatLngBounds;

namespace util {

// Helper class to stream tile-cover results per row
class TileCover {
public:
    TileCover(const LatLngBounds&, int32_t z);
    // When project == true, projects the geometry points to tile coordinates
    TileCover(const Geometry<double>&, int32_t z, bool project = true);
    ~TileCover();

    optional<UnwrappedTileID> next();
    bool hasNext();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

int32_t coveringZoomLevel(double z, style::SourceType type, uint16_t tileSize);

std::vector<UnwrappedTileID> tileCover(const TransformState&, int32_t z);
std::vector<UnwrappedTileID> tileCover(const LatLngBounds&, int32_t z);
std::vector<UnwrappedTileID> tileCover(const Geometry<double>&, int32_t z);

// Compute only the count of tiles needed for tileCover
uint64_t tileCount(const LatLngBounds&, uint8_t z);
uint64_t tileCount(const Geometry<double>&, uint8_t z);

} // namespace util
} // namespace mbgl
