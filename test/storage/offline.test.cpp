#include <mbgl/storage/offline.hpp>
#include <mbgl/tile/tile_id.hpp>

#include <gtest/gtest.h>

using namespace mbgl;
using SourceType = mbgl::style::SourceType;


TEST(OfflineTilePyramidRegionDefinition, EncodeDecode) {
    OfflineTilePyramidRegionDefinition region("mapbox://style", LatLngBounds::hull({ 37.6609, -122.5744 }, { 37.8271, -122.3204 }), 0, 20, 1.0, true);

    auto encoded = encodeOfflineRegionDefinition(region);
    auto decoded = decodeOfflineRegionDefinition(encoded).get<OfflineTilePyramidRegionDefinition>();
    
    EXPECT_EQ(decoded.styleURL, region.styleURL);
    EXPECT_EQ(decoded.minZoom, region.minZoom);
    EXPECT_EQ(decoded.maxZoom, region.maxZoom);
    EXPECT_EQ(decoded.pixelRatio, region.pixelRatio);
    EXPECT_EQ(decoded.bounds.southwest(), region.bounds.southwest());
    EXPECT_EQ(decoded.bounds.northeast(), region.bounds.northeast());
    EXPECT_EQ(decoded.includeIdeographs, region.includeIdeographs);
}

TEST(OfflineGeometryRegionDefinition, EncodeDecode) {
    OfflineGeometryRegionDefinition region("mapbox://style", Point<double>(-122.5744, 37.6609), 0, 2, 1.0, false);

    auto encoded = encodeOfflineRegionDefinition(region);
    auto decoded = decodeOfflineRegionDefinition(encoded).get<OfflineGeometryRegionDefinition>();
    
    EXPECT_EQ(decoded.styleURL, region.styleURL);
    EXPECT_EQ(decoded.minZoom, region.minZoom);
    EXPECT_EQ(decoded.maxZoom, region.maxZoom);
    EXPECT_EQ(decoded.pixelRatio, region.pixelRatio);
    EXPECT_EQ(decoded.geometry, region.geometry);
    EXPECT_EQ(decoded.includeIdeographs, region.includeIdeographs);
}
