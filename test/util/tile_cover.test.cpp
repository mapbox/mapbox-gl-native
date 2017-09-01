#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/map/transform.hpp>

#include <gtest/gtest.h>

using namespace mbgl;

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
    transform.resize({ 512, 512 });
    // slightly offset center so that tile order is better defined
    transform.setLatLng({ 0.1, -0.1 });
    transform.setZoom(2);
    transform.setAngle(5.0);
    transform.setPitch(40.0 * M_PI / 180.0);

    EXPECT_EQ((std::vector<UnwrappedTileID>{
                  { 2, 1, 2 }, { 2, 1, 1 }, { 2, 2, 2 }, { 2, 2, 1 }, { 2, 3, 2 }
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

TEST(TileCount, SanFranciscoZ10) {
    EXPECT_EQ(4u, util::tileCount(sanFrancisco, 10, util::tileSize));
}

TEST(TileCount, SanFranciscoZ22) {
    EXPECT_EQ(7254450u, util::tileCount(sanFrancisco, 22, util::tileSize));
}

