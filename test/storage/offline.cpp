#include <mbgl/storage/offline.hpp>
#include <mbgl/source/source_info.hpp>
#include <mbgl/map/tile_id.hpp>

#include <gtest/gtest.h>

using namespace mbgl;

static const LatLngBounds sanFrancisco = LatLngBounds::hull(
    { 37.6609, -122.5744 },
    { 37.8271, -122.3204 });

static const LatLngBounds sanFranciscoWrapped = LatLngBounds::hull(
    { 37.6609, 238.5744 },
    { 37.8271, 238.3204 });

TEST(OfflineTilePyramidRegionDefinition, TileCoverEmpty) {
    OfflineTilePyramidRegionDefinition region("", LatLngBounds::empty(), 0, 20, 1.0);
    SourceInfo info;

    auto result = region.tileCover(SourceType::Vector, 512, info);
    ASSERT_TRUE(result.empty());
}

TEST(OfflineTilePyramidRegionDefinition, TileCoverZoomIntersection) {
    OfflineTilePyramidRegionDefinition region("", sanFrancisco, 2, 2, 1.0);
    SourceInfo info;

    info.minZoom = 0;
    auto resultIntersection = region.tileCover(SourceType::Vector, 512, info);
    ASSERT_EQ(1, resultIntersection.size());

    info.minZoom = 3;
    auto resultNoIntersection = region.tileCover(SourceType::Vector, 512, info);
    ASSERT_TRUE(resultNoIntersection.empty());
}

TEST(OfflineTilePyramidRegionDefinition, TileCoverTileSize) {
    OfflineTilePyramidRegionDefinition region("", LatLngBounds::world(), 0, 0, 1.0);
    SourceInfo info;

    auto result512 = region.tileCover(SourceType::Vector, 512, info);
    ASSERT_EQ(1, result512.size());
    ASSERT_EQ(0, result512[0].z);

    auto result256 = region.tileCover(SourceType::Vector, 256, info);
    ASSERT_EQ(4, result256.size());
    ASSERT_EQ(1, result256[0].z);
}

TEST(OfflineTilePyramidRegionDefinition, TileCoverZoomRounding) {
    OfflineTilePyramidRegionDefinition region("", sanFrancisco, 0.6, 0.7, 1.0);
    SourceInfo info;

    auto resultVector = region.tileCover(SourceType::Vector, 512, info);
    ASSERT_EQ(1, resultVector.size());
    ASSERT_EQ(0, resultVector[0].z);

    auto resultRaster = region.tileCover(SourceType::Raster, 512, info);
    ASSERT_EQ(1, resultRaster.size());
    ASSERT_EQ(1, resultRaster[0].z);
}

TEST(OfflineTilePyramidRegionDefinition, TileCoverWrapped) {
    OfflineTilePyramidRegionDefinition region("", sanFranciscoWrapped, 0, 0, 1.0);
    SourceInfo info;

    auto result = region.tileCover(SourceType::Vector, 512, info);
    ASSERT_EQ(1, result.size());
    ASSERT_EQ(0, result[0].z);
    ASSERT_EQ(0, result[0].x);
    ASSERT_EQ(0, result[0].y);
}
