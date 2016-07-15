#include <mbgl/storage/offline.hpp>
#include <mbgl/tile/tile_id.hpp>

#include <gtest/gtest.h>

using namespace mbgl;

using OfflineTilePyramidRegionDefinition = OfflineFixedGeometryRegionDefinition<LatLngBounds>;

static const LatLngBounds sanFrancisco =
    LatLngBounds::hull({ 37.6609, -122.5744 }, { 37.8271, -122.3204 });

static const LatLngBounds sanFranciscoWrapped =
    LatLngBounds::hull({ 37.6609, 238.5744 }, { 37.8271, 238.3204 });

TEST(OfflineRegionDefinition, TileCoverEmpty) {
    OfflineTilePyramidRegionDefinition region("", LatLngBounds::empty(), {0, 20}, 1.0);

    EXPECT_EQ((std::vector<CanonicalTileID>{}), tileCover(region, SourceType::Vector, 512, { 0, 22 }));
}

TEST(OfflineRegionDefinition, TileCoverZoomIntersection) {
    OfflineTilePyramidRegionDefinition region("", sanFrancisco, {2, 2}, 1.0);

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 2, 0, 1 } }),
              tileCover(region, SourceType::Vector, 512, { 0, 22 }));

    EXPECT_EQ((std::vector<CanonicalTileID>{}), tileCover(region, SourceType::Vector, 512, { 3, 22 }));
}

TEST(OfflineRegionDefinition, TileCoverTileSize) {
    OfflineTilePyramidRegionDefinition region("", LatLngBounds::world(), {0, 0}, 1.0);

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 0, 0, 0 } }),
              tileCover(region, SourceType::Vector, 512, { 0, 22 }));

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 1, 0, 0 }, { 1, 0, 1 }, { 1, 1, 0 }, { 1, 1, 1 } }),
              tileCover(region, SourceType::Vector, 256, { 0, 22 }));
}

TEST(OfflineRegionDefinition, TileCoverZoomRounding) {
    OfflineTilePyramidRegionDefinition region("", sanFrancisco, {0.6, 0.7}, 1.0);

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 0, 0, 0 } }),
              tileCover(region, SourceType::Vector, 512, { 0, 22 }));

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 1, 0, 0 } }),
              tileCover(region, SourceType::Raster, 512, { 0, 22 }));
}

TEST(OfflineRegionDefinition, TileCoverWrapped) {
    OfflineTilePyramidRegionDefinition region("", sanFranciscoWrapped, {0, 0}, 1.0);

    EXPECT_EQ((std::vector<CanonicalTileID>{ { 0, 0, 0 } }),
              tileCover(region, SourceType::Vector, 512, { 0, 22 }));
}

TEST(OfflinePolygonRegionDefinition, EncodeDecode) {
    Polygon<double> polygon { {
        { 1, 1, },
        { 2, 2, },
        { 1.5, 2.5 }
    } };
    OfflineFixedGeometryRegionDefinition<Polygon<double>> region("", polygon, { 2, 2 }, 1.0);
    
    auto encoded = encodeOfflineRegionDefinition(region);
    auto decoded = decodeOfflineRegionDefinition(encoded).get<OfflineFixedGeometryRegionDefinition<Polygon<double>>>();
    
    EXPECT_EQ(region.maxZoom, decoded.maxZoom);
    EXPECT_EQ(region.minZoom, decoded.minZoom);
    EXPECT_EQ(region.pixelRatio, decoded.pixelRatio);
    EXPECT_EQ(region.styleURL, decoded.styleURL);
    EXPECT_EQ(region.geometry, decoded.geometry);
}

