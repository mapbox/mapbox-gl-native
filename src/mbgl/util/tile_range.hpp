#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/range.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/projection.hpp>

namespace mbgl {

namespace util {

class TileRange {
public:
    Range<Point<double>> range;
    uint8_t z;

    // Compute the range of tiles covered by the bounds.
    static TileRange fromLatLngBounds(const LatLngBounds& bounds, uint8_t z) {
        auto swProj = Projection::project(bounds.southwest().wrapped(), z);
        auto ne = bounds.northeast();
        auto neProj = Projection::project(ne.longitude() > util::LONGITUDE_MAX ? ne.wrapped() : ne , z);
        const auto minX = std::floor(swProj.x);
        const auto maxX = std::ceil(neProj.x);
        const auto minY = std::floor(neProj.y);
        const auto maxY = std::ceil(swProj.y);
        return TileRange({ {minX, minY}, {maxX, maxY} }, z);
    }

    bool contains(const CanonicalTileID& tileID) {
        return z == tileID.z &&
        (range.min.x >= range.max.x ? //For wrapped bounds
            tileID.x >= range.min.x || tileID.x < range.max.x :
            tileID.x < range.max.x && tileID.x >= range.min.x) &&
        tileID.y < range.max.y &&
        tileID.y >= range.min.y;
    }

private:
    TileRange(Range<Point<double>> range_, uint8_t z_)
     : range(range_),
     z(z_) {
    }
    
};

} // namespace util
} // namespace mbgl
