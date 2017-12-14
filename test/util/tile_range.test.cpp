
#include <mbgl/util/tile_range.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/map/transform.hpp>

#include <gtest/gtest.h>

using namespace mbgl;

TEST(TileRange, Contains) {
    {
        auto range = util::TileRange::fromLatLngBounds(LatLngBounds::world(), 0);
        EXPECT_TRUE(range.contains(CanonicalTileID(0, 0, 0)));
        EXPECT_FALSE(range.contains(CanonicalTileID(10, 0, 0)));
    }
    {
        const LatLngBounds bounds{ CanonicalTileID(3, 7, 0) };
        auto range = util::TileRange::fromLatLngBounds(bounds, 3);
        EXPECT_TRUE(range.contains(CanonicalTileID(3, 7, 0)));
    }
    {
        const LatLngBounds bounds{ CanonicalTileID(10, 162, 395) };
        auto range = util::TileRange::fromLatLngBounds(bounds, 10);
        EXPECT_TRUE(range.contains(CanonicalTileID(10,162,395)));
    }
    {
        auto bounds = LatLngBounds::hull({ 37.6609, -122.5744 }, { 37.8271, -122.3204 });
        auto range = util::TileRange::fromLatLngBounds(bounds, 13);
        EXPECT_FALSE(range.contains(CanonicalTileID(13,1316,3100)));
        EXPECT_TRUE(range.contains(CanonicalTileID(13, 1310, 3166)));
    }
    {
        auto wrappedBounds = LatLngBounds::hull({ 37.6609, 237.6796 }, { 37.8271, 237.4256 });
        auto range = util::TileRange::fromLatLngBounds(wrappedBounds, 13);
        EXPECT_FALSE(range.contains(CanonicalTileID(13,1316,3100)));
        EXPECT_TRUE(range.contains(CanonicalTileID(13, 1310, 3166)));
    }
}
