#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/map/transform.hpp>

#include <gtest/gtest.h>

using namespace mbgl;

namespace {
    
bool containsTile(const std::vector<UnwrappedTileID>& tiles, uint8_t z, uint32_t x, uint32_t y) {
    UnwrappedTileID target(z, x, y);
    return std::find(tiles.cbegin(), tiles.cend(), target) != tiles.cend();
}

Polygon<double> toLatLng(const Polygon<double>& polygon, double zoom) {
    const TransformState state;
    Polygon<double> result;
    for (const LinearRing<double>& ring : polygon) {
        result.emplace_back();
        for (const Point<double>& point : ring) {
            TileCoordinate coord{point, zoom};
            LatLng latLng = coord.toLatLng(state);
            result.back().emplace_back(latLng.longitude, latLng.latitude);
        }
    }
    return result;
}
    
}

TEST(TileCover, Empty) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{}), util::tileCover(LatLngBounds::empty(), 0));
}

TEST(TileCover, Arctic) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{}),
              util::tileCover(LatLngBounds::hull({ 86, -180 }, { 90, 180 }), 0));
}

TEST(TileCover, Antarctic) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{}),
              util::tileCover(LatLngBounds::hull({ -86, -180 }, { -90, 180 }), 0));
}

TEST(TileCover, WorldZ0) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{
                  { 0, 0, 0 },
              }),
              util::tileCover(LatLngBounds::world(), 0));
}

TEST(TileCover, Pitch) {
    Transform transform;
    transform.resize({ { 512, 512 } });
    transform.setZoom(2);
    transform.setPitch(40.0 * M_PI / 180.0);

    EXPECT_EQ((std::vector<UnwrappedTileID>{
                  { 2, 1, 1 }, { 2, 1, 2 }, { 2, 2, 1 }, { 2, 2, 2 },
              }),
              util::tileCover(transform.getState(), 2));
}

TEST(TileCover, WorldZ1) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{
                  { 1, 0, 0 }, { 1, 0, 1 }, { 1, 1, 0 }, { 1, 1, 1 },
              }),
              util::tileCover(LatLngBounds::world(), 1));
}

TEST(TileCover, SingletonZ0) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{}),
              util::tileCover(LatLngBounds::singleton({ 0, 0 }), 0));
}

TEST(TileCover, SingletonZ1) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{}),
              util::tileCover(LatLngBounds::singleton({ 0, 0 }), 1));
}

static const LatLngBounds sanFrancisco =
    LatLngBounds::hull({ 37.6609, -122.5744 }, { 37.8271, -122.3204 });

TEST(TileCover, SanFranciscoZ0) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{
                  { 0, 0, 0 },
              }),
              util::tileCover(sanFrancisco, 0));
}

TEST(TileCover, SanFranciscoZ10) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{
                  { 10, 163, 395 }, { 10, 163, 396 }, { 10, 164, 395 }, { 10, 164, 396 },

              }),
              util::tileCover(sanFrancisco, 10));
}

static const LatLngBounds sanFranciscoWrapped =
    LatLngBounds::hull({ 37.6609, 238.5744 }, { 37.8271, 238.3204 });

TEST(TileCover, SanFranciscoZ0Wrapped) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{ { 0, 1, 0 } }),
              util::tileCover(sanFranciscoWrapped, 0));
}

TEST(TileCover, PolygonAboveArcticCircleZ1) {
    Polygon<double> polygon { {
        { 0.0, 87.0 },
        { 30.0, 87.0 },
        { 40.0, 88.0 },
        { -10.0, 88.0 }
    } };
    
    std::vector<UnwrappedTileID> tiles = util::tileCover(polygon, 1);
    
    EXPECT_EQ(0, tiles.size());
}


TEST(TileCover, SquarePolygonZ1) {
    // A simple square that crosses tile boundaries
    Polygon<double> polygon { {
        { 0.5, 0.5 },
        { 1.5, 0.5 },
        { 1.5, 1.5 },
        { 0.5, 1.5 }
    } };
    Polygon<double> latLng = toLatLng(polygon, 1);
    
    std::vector<UnwrappedTileID> tiles = util::tileCover(latLng, 1);
    
    EXPECT_EQ(4, tiles.size());
    EXPECT_TRUE(containsTile(tiles, 1, 0, 0));
    EXPECT_TRUE(containsTile(tiles, 1, 0, 1));
    EXPECT_TRUE(containsTile(tiles, 1, 1, 0));
    EXPECT_TRUE(containsTile(tiles, 1, 1, 1));
}

TEST(TileCover, UShapeZ2) {
    // A block "U"
    Polygon<double> polygon { {
        { 0.1, 0.1 },
        { 2.9, 0.1 },
        { 2.9, 2.9 },
        { 2.1, 2.9 },
        { 2.1, 0.9 },
        { 0.9, 0.9 },
        { 0.9, 2.9 },
        { 0.1, 2.9 }
    } };
    Polygon<double> latLng = toLatLng(polygon, 2);
    
    std::vector<UnwrappedTileID> tiles = util::tileCover(latLng, 2);
    
    EXPECT_EQ(7, tiles.size());
    EXPECT_TRUE(containsTile(tiles, 2, 0, 0));
    EXPECT_TRUE(containsTile(tiles, 2, 0, 1));
    EXPECT_TRUE(containsTile(tiles, 2, 0, 2));
    EXPECT_TRUE(containsTile(tiles, 2, 1, 0));
    EXPECT_TRUE(containsTile(tiles, 2, 2, 0));
    EXPECT_TRUE(containsTile(tiles, 2, 2, 1));
    EXPECT_TRUE(containsTile(tiles, 2, 2, 2));
}

TEST(TileCover, DiagonalEdgeZ2) {
    // A concave shape that crosses tile boundaries on diagonals
    Polygon<double> polygon { {
        { 2.9, 0.9 },
        { 3.1, 2.0 },
        { 2.9, 3.1 },
        { 3.6, 3.1 },
        { 3.6, 0.9 }
    } };
    Polygon<double> latLng = toLatLng(polygon, 2);
    
    std::vector<UnwrappedTileID> tiles = util::tileCover(latLng, 2);
    
    EXPECT_EQ(8, tiles.size());
    EXPECT_TRUE(containsTile(tiles, 2, 2, 0));
    EXPECT_TRUE(containsTile(tiles, 2, 2, 1));
    EXPECT_TRUE(containsTile(tiles, 2, 2, 2));
    EXPECT_TRUE(containsTile(tiles, 2, 2, 3));
    EXPECT_TRUE(containsTile(tiles, 2, 3, 0));
    EXPECT_TRUE(containsTile(tiles, 2, 3, 1));
    EXPECT_TRUE(containsTile(tiles, 2, 3, 2));
    EXPECT_TRUE(containsTile(tiles, 2, 3, 3));
}

TEST(TileCover, HoleContainedInTileZ1) {
    // A shape with a hole that is entirely contained within a single tile
    Polygon<double> polygon {
        {
            { 0.2, 0.2 },
            { 0.8, 0.2 },
            { 0.8, 0.8 },
            { 0.2, 0.8 }
        },
        {
            { 0.4, 0.4 },
            { 0.4, 0.6 },
            { 0.6, 0.6 },
            { 0.6, 0.4 }
        }
    };
    Polygon<double> latLng = toLatLng(polygon, 1);
    
    std::vector<UnwrappedTileID> tiles = util::tileCover(latLng, 1);
    
    EXPECT_EQ(1, tiles.size());
    EXPECT_TRUE(containsTile(tiles, 1, 0, 0));
}

TEST(TileCover, HoleSpanningTileBoundaryZ2) {
    // A shape with a hole that crosses tile boundaries, but is not so large
    // that it causes any tiles to actually be excluded
    Polygon<double> polygon {
        {
            { 0.2, 0.2 },
            { 1.8, 0.2 },
            { 1.8, 1.8 },
            { 0.2, 1.8 }
        },
        {
            { 0.4, 0.4 },
            { 0.4, 1.6 },
            { 1.6, 1.6 },
            { 1.6, 0.4 }
        }
    };
    Polygon<double> latLng = toLatLng(polygon, 2);
    
    std::vector<UnwrappedTileID> tiles = util::tileCover(latLng, 2);
    
    EXPECT_EQ(4, tiles.size());
    EXPECT_TRUE(containsTile(tiles, 2, 0, 0));
    EXPECT_TRUE(containsTile(tiles, 2, 0, 1));
    EXPECT_TRUE(containsTile(tiles, 2, 1, 0));
    EXPECT_TRUE(containsTile(tiles, 2, 1, 1));
}

TEST(TileCover, HoleLargeEnoughToExcludeTileZ2) {
    // A shape with a hole that is large enough to cause tiles to actually be
    // excluded.
    Polygon<double> polygon {
        {
            { 0.2, 0.2 },
            { 3.8, 0.2 },
            { 3.8, 3.8 },
            { 0.2, 3.8 }
        },
        {
            { 0.4, 0.4 },
            { 0.4, 3.6 },
            { 3.6, 3.6 },
            { 3.6, 0.4 }
        }
    };
    Polygon<double> latLng = toLatLng(polygon, 2);
    
    std::vector<UnwrappedTileID> tiles = util::tileCover(latLng, 2);
    
    EXPECT_EQ(12, tiles.size());
    EXPECT_TRUE(containsTile(tiles, 2, 0, 0));
    EXPECT_TRUE(containsTile(tiles, 2, 0, 1));
    EXPECT_TRUE(containsTile(tiles, 2, 0, 2));
    EXPECT_TRUE(containsTile(tiles, 2, 0, 3));
    EXPECT_TRUE(containsTile(tiles, 2, 3, 0));
    EXPECT_TRUE(containsTile(tiles, 2, 3, 1));
    EXPECT_TRUE(containsTile(tiles, 2, 3, 2));
    EXPECT_TRUE(containsTile(tiles, 2, 3, 3));
    EXPECT_TRUE(containsTile(tiles, 2, 1, 0));
    EXPECT_TRUE(containsTile(tiles, 2, 2, 0));
    EXPECT_TRUE(containsTile(tiles, 2, 1, 3));
    EXPECT_TRUE(containsTile(tiles, 2, 2, 3));
}

TEST(TileCover, PolygonBeyondAntimeridianZ0) {
    Polygon<double> latLng { {
        { 190, 10 },
        { 200, 10 },
        { 200, 30 },
        { 190, 30 }
    } };
    
    std::vector<UnwrappedTileID> tiles = util::tileCover(latLng, 0);
    
    EXPECT_EQ(1, tiles.size());
    EXPECT_EQ(0, tiles[0].canonical.z);
    EXPECT_EQ(0, tiles[0].canonical.x);
    EXPECT_EQ(0, tiles[0].canonical.y);
    EXPECT_EQ(1, tiles[0].wrap);
}

TEST(TileCover, PolygonSpanningAntimeridianZ1) {
    Polygon<double> latLng { {
        { 170, -20 },
        { 190, -20 },
        { 190, 20 },
        { 170, 20 }
    } };
    
    std::vector<UnwrappedTileID> tiles = util::tileCover(latLng, 1);
    
    EXPECT_EQ(4, tiles.size());
    EXPECT_TRUE(std::find_if(tiles.cbegin(), tiles.cend(), [](const UnwrappedTileID& tile) {
        return tile.canonical.x == 0 && tile.canonical.y == 0 && tile.wrap == 1;
    }) != tiles.end());
    EXPECT_TRUE(std::find_if(tiles.cbegin(), tiles.cend(), [](const UnwrappedTileID& tile) {
        return tile.canonical.x == 1 && tile.canonical.y == 0 && tile.wrap == 0;
    }) != tiles.end());
    EXPECT_TRUE(std::find_if(tiles.cbegin(), tiles.cend(), [](const UnwrappedTileID& tile) {
        return tile.canonical.x == 0 && tile.canonical.y == 1 && tile.wrap == 1;
    }) != tiles.end());
    EXPECT_TRUE(std::find_if(tiles.cbegin(), tiles.cend(), [](const UnwrappedTileID& tile) {
        return tile.canonical.x == 1 && tile.canonical.y == 1 && tile.wrap == 0;
    }) != tiles.end());
}
