#include <mbgl/test/util.hpp>

#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/util/constants.hpp>


using namespace mbgl;

TEST(Tile, TilePoint) {
    std::vector<Point<double>> edges {
        { 0.5, 0.5 }, { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 }
    };

    for (uint8_t zoom = 0; zoom <= 16; ++zoom) {
        uint32_t maxTilesPerAxis = std::pow(2, zoom);
        for (const auto& edge : edges) {
            uint32_t tileX = edge.x == 0 ? 0 : edge.x == 1 ? maxTilesPerAxis - 1 : (maxTilesPerAxis / 2.0) - 1;
            uint32_t tileY = edge.y == 0 ? 0 : edge.y == 1 ? maxTilesPerAxis - 1 : (maxTilesPerAxis / 2.0) - 1;
            UnwrappedTileID unwrapped(0, CanonicalTileID { zoom, tileX, tileY });

            auto tilePointX = ((edge.x * maxTilesPerAxis) - tileX) * util::EXTENT;
            auto tilePointY = ((edge.y * maxTilesPerAxis) - tileY) * util::EXTENT;
            TilePoint point = Tile::pointFromTileCoordinate(unwrapped, edge);
            ASSERT_DOUBLE_EQ(point.x, tilePointX);
            ASSERT_DOUBLE_EQ(point.y, tilePointY);
        }
    }
}
