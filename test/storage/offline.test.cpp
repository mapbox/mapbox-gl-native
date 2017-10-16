#include <mbgl/storage/offline.hpp>
#include <mbgl/tile/tile_id.hpp>

#include <gtest/gtest.h>

using namespace mbgl;
using SourceType = mbgl::style::SourceType;

static const LatLngBounds sanFrancisco =
    LatLngBounds::hull({ 37.6609, -122.5744 }, { 37.8271, -122.3204 });

static const LatLngBounds sanFranciscoWrapped =
    LatLngBounds::hull({ 37.6609, 238.5744 }, { 37.8271, 238.3204 });

TEST(OfflineTilePyramidRegionDefinition, TileCoverEmpty) {
    OfflineTilePyramidRegionDefinition region("", LatLngBounds::empty(), 0, 20, 1.0);

    EXPECT_EQ((std::vector<CanonicalTileID>{}), region.tileCover(SourceType::Vector, 512, { 0, 22 }));
}

TEST(OfflineTilePyramidRegionDefinition, TileCoverZoomIntersection) {
    OfflineTilePyramidRegionDefinition region("", sanFrancisco, 2, 2, 1.0);

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 2, 0, 1 } }),
              region.tileCover(SourceType::Vector, 512, { 0, 22 }));

    EXPECT_EQ((std::vector<CanonicalTileID>{}), region.tileCover(SourceType::Vector, 512, { 3, 22 }));
}

TEST(OfflineTilePyramidRegionDefinition, TileCoverTileSize) {
    OfflineTilePyramidRegionDefinition region("", LatLngBounds::world(), 0, 0, 1.0);

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 0, 0, 0 } }),
              region.tileCover(SourceType::Vector, 512, { 0, 22 }));

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 1, 0, 0 }, { 1, 0, 1 }, { 1, 1, 0 }, { 1, 1, 1 } }),
              region.tileCover(SourceType::Vector, 256, { 0, 22 }));
}

TEST(OfflineTilePyramidRegionDefinition, TileCoverZoomRounding) {
    OfflineTilePyramidRegionDefinition region("", sanFrancisco, 0.6, 0.7, 1.0);

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 0, 0, 0 } }),
              region.tileCover(SourceType::Vector, 512, { 0, 22 }));

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 1, 0, 0 } }),
              region.tileCover(SourceType::Raster, 512, { 0, 22 }));
}

TEST(OfflineTilePyramidRegionDefinition, TileCoverWrapped) {
    OfflineTilePyramidRegionDefinition region("", sanFranciscoWrapped, 0, 0, 1.0);

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 0, 0, 0 } }),
              region.tileCover(SourceType::Vector, 512, { 0, 22 }));
}

TEST(OfflineTilePyramidRegionDefinition, TileCount) {
    OfflineTilePyramidRegionDefinition region("", sanFranciscoWrapped, 0, 22, 1.0);

    //These numbers match the count from tileCover().size().
    EXPECT_EQ(38424u, region.tileCount(SourceType::Vector, 512, { 10, 18 }));
    EXPECT_EQ(9675240u, region.tileCount(SourceType::Vector, 512, { 3, 22 }));
}
