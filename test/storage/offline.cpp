#include <mbgl/storage/offline.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/tile/tile_id.hpp>

#include <gtest/gtest.h>

using namespace mbgl;

static const LatLngBounds sanFrancisco =
    LatLngBounds::hull({ 37.6609, -122.5744 }, { 37.8271, -122.3204 });

static const LatLngBounds sanFranciscoWrapped =
    LatLngBounds::hull({ 37.6609, 238.5744 }, { 37.8271, 238.3204 });

TEST(OfflineTilePyramidRegionDefinition, TileCoverEmpty) {
    OfflineTilePyramidRegionDefinition region("", LatLngBounds::empty(), 0, 20, 1.0);
    Tileset tileset;

    EXPECT_EQ((std::vector<CanonicalTileID>{}), region.tileCover(SourceType::Vector, 512, tileset));
}

TEST(OfflineTilePyramidRegionDefinition, TileCoverZoomIntersection) {
    OfflineTilePyramidRegionDefinition region("", sanFrancisco, 2, 2, 1.0);
    Tileset tileset;

    tileset.minZoom = 0;
    EXPECT_EQ((std::vector<CanonicalTileID>{ { 2, 0, 1 } }),
              region.tileCover(SourceType::Vector, 512, tileset));

    tileset.minZoom = 3;
    EXPECT_EQ((std::vector<CanonicalTileID>{}), region.tileCover(SourceType::Vector, 512, tileset));
}

TEST(OfflineTilePyramidRegionDefinition, TileCoverTileSize) {
    OfflineTilePyramidRegionDefinition region("", LatLngBounds::world(), 0, 0, 1.0);
    Tileset tileset;

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 0, 0, 0 } }),
              region.tileCover(SourceType::Vector, 512, tileset));

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 1, 0, 0 }, { 1, 0, 1 }, { 1, 1, 0 }, { 1, 1, 1 } }),
              region.tileCover(SourceType::Vector, 256, tileset));
}

TEST(OfflineTilePyramidRegionDefinition, TileCoverZoomRounding) {
    OfflineTilePyramidRegionDefinition region("", sanFrancisco, 0.6, 0.7, 1.0);
    Tileset tileset;

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 0, 0, 0 } }),
              region.tileCover(SourceType::Vector, 512, tileset));

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 1, 0, 0 } }),
              region.tileCover(SourceType::Raster, 512, tileset));
}

TEST(OfflineTilePyramidRegionDefinition, TileCoverWrapped) {
    OfflineTilePyramidRegionDefinition region("", sanFranciscoWrapped, 0, 0, 1.0);
    Tileset tileset;

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 0, 0, 0 } }),
              region.tileCover(SourceType::Vector, 512, tileset));
}
