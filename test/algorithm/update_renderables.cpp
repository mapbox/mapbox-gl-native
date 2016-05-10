#include <mbgl/test/util.hpp>
#include "mock.hpp"

#include <mbgl/algorithm/update_renderables_impl.hpp>

using namespace mbgl;

TEST(UpdateRenderables, SingleTile) {
    MockSource source;
    source.idealTiles.emplace(UnwrappedTileID{ 1, 1, 1 });

    // Make sure that we're getting the tile back.
    auto tile_1_1_1 = source.createTileData(OverscaledTileID{ 1, 1, 1 });
    tile_1_1_1->ready = true;

    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 1, 1, 1 }, MockRenderable{ { 1, 1, 1 }, *tile_1_1_1 } },
              }),
              source.renderables);

    // Check a repeated render with the same data.
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 1, 1, 1 }, MockRenderable{ { 1, 1, 1 }, *tile_1_1_1 } },
              }),
              source.renderables);

    // Insert a tile we don't have data for.
    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 1 });
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 1, 1, 1 }, MockRenderable{ { 1, 1, 1 }, *tile_1_1_1 } },
              }),
              source.renderables);

    // Now insert the missing tile and check that we're rendering it.
    auto tile_1_0_1 = source.createTileData(OverscaledTileID{ 1, 0, 1 });
    tile_1_0_1->ready = true;
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 1, 0, 1 }, MockRenderable{ { 1, 0, 1 }, *tile_1_0_1 } },
                  { { 1, 1, 1 }, MockRenderable{ { 1, 1, 1 }, *tile_1_1_1 } },
              }),
              source.renderables);

    // Insert another tile, and another bucket that has a different name and check that we're not
    // using it.
    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 0 });
    auto tile_1_0_0 = source.createTileData(OverscaledTileID{ 1, 0, 0 });

    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 1, 0, 1 }, MockRenderable{ { 1, 0, 1 }, *tile_1_0_1 } },
                  { { 1, 1, 1 }, MockRenderable{ { 1, 1, 1 }, *tile_1_1_1 } },
              }),
              source.renderables);

    // Then, add the bucket and check that it's getting used.
    tile_1_0_0->ready = true;
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 1, 0, 0 }, MockRenderable{ { 1, 0, 0 }, *tile_1_0_0 } },
                  { { 1, 0, 1 }, MockRenderable{ { 1, 0, 1 }, *tile_1_0_1 } },
                  { { 1, 1, 1 }, MockRenderable{ { 1, 1, 1 }, *tile_1_1_1 } },
              }),
              source.renderables);
}

TEST(UpdateRenderables, UseParentTile) {
    MockSource source;
    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 1 });
    source.idealTiles.emplace(UnwrappedTileID{ 1, 1, 0 });
    source.idealTiles.emplace(UnwrappedTileID{ 1, 1, 1 });

    // Make sure that we're getting the tile back.
    auto tile_0_0_0 = source.createTileData(OverscaledTileID{ 0, 0, 0 });
    tile_0_0_0->ready = true;
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 0, 0, 0 }, MockRenderable{ { 0, 0, 0 }, *tile_0_0_0 } },
              }),
              source.renderables);
}

TEST(UpdateRenderables, DontUseWrongParentTile) {
    MockSource source;
    source.idealTiles.emplace(UnwrappedTileID{ 2, 0, 0 });

    auto tile_1_1_0 = source.createTileData(OverscaledTileID{ 1, 1, 0 });
    tile_1_1_0->ready = true;
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 2);
    EXPECT_EQ(decltype(source.renderables)({}), source.renderables);

    // Add a new child tile and check that it is now used.
    source.idealTiles.emplace(UnwrappedTileID{ 2, 2, 0 });
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 2);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 1, 1, 0 }, MockRenderable{ { 1, 1, 0 }, *tile_1_1_0 } },
              }),
              source.renderables);
}

TEST(UpdateRenderables, UseParentTileWhenChildNotReady) {
    MockSource source;
    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 1 });

    auto tile_0_0_0 = source.createTileData(OverscaledTileID{ 0, 0, 0 });
    tile_0_0_0->ready = true;

    auto tile_1_0_1 = source.createTileData(OverscaledTileID{ 1, 0, 1 });
    // Don't create bucket.

    // Make sure that it renders the parent tile.
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 0, 0, 0 }, MockRenderable{ { 0, 0, 0 }, *tile_0_0_0 } },
              }),
              source.renderables);

    // Now insert the bucket and make sure we're now using the matching tile
    tile_1_0_1->ready = true;
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 1, 0, 1 }, MockRenderable{ { 1, 0, 1 }, *tile_1_0_1 } },
              }),
              source.renderables);
}

TEST(UpdateRenderables, UseOverlappingParentTile) {
    MockSource source;
    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 0 });
    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 1 });

    auto tile_0_0_0 = source.createTileData(OverscaledTileID{ 0, 0, 0 });
    tile_0_0_0->ready = true;

    auto tile_1_0_1 = source.createTileData(OverscaledTileID{ 1, 0, 1 });
    tile_1_0_1->ready = true;

    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 0, 0, 0 }, MockRenderable{ { 0, 0, 0 }, *tile_0_0_0 } },
                  { { 1, 0, 1 }, MockRenderable{ { 1, 0, 1 }, *tile_1_0_1 } },
              }),
              source.renderables);
}

TEST(UpdateRenderables, UseChildTiles) {
    MockSource source;
    source.idealTiles.emplace(UnwrappedTileID{ 0, 0, 0 });

    auto tile_1_0_0 = source.createTileData(OverscaledTileID{ 1, 0, 0 });
    tile_1_0_0->ready = true;
    auto tile_1_1_0 = source.createTileData(OverscaledTileID{ 1, 1, 0 });
    tile_1_1_0->ready = true;

    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 0);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 1, 0, 0 }, MockRenderable{ { 1, 0, 0 }, *tile_1_0_0 } },
                  { { 1, 1, 0 }, MockRenderable{ { 1, 1, 0 }, *tile_1_1_0 } },
              }),
              source.renderables);
}

TEST(UpdateRenderables, PreferChildTiles) {
    MockSource source;
    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 0 });

    auto tile_0_0_0 = source.createTileData(OverscaledTileID{ 0, 0, 0 });
    tile_0_0_0->ready = true;
    auto tile_2_0_0 = source.createTileData(OverscaledTileID{ 2, 0, 0 });
    tile_2_0_0->ready = true;

    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 0, 0, 0 }, MockRenderable{ { 0, 0, 0 }, *tile_0_0_0 } },
                  { { 2, 0, 0 }, MockRenderable{ { 2, 0, 0 }, *tile_2_0_0 } },
              }),
              source.renderables);

    // Now add more children to cover the ideal tile fully, until it is covered fully, and verify
    // that the parent doesn't get rendered.
    auto tile_2_0_1 = source.createTileData(OverscaledTileID{ 2, 0, 1 });
    tile_2_0_1->ready = true;
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 0, 0, 0 }, MockRenderable{ { 0, 0, 0 }, *tile_0_0_0 } },
                  { { 2, 0, 0 }, MockRenderable{ { 2, 0, 0 }, *tile_2_0_0 } },
                  { { 2, 0, 1 }, MockRenderable{ { 2, 0, 1 }, *tile_2_0_1 } },
              }),
              source.renderables);

    auto tile_2_1_0 = source.createTileData(OverscaledTileID{ 2, 1, 0 });
    tile_2_1_0->ready = true;
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 0, 0, 0 }, MockRenderable{ { 0, 0, 0 }, *tile_0_0_0 } },
                  { { 2, 0, 0 }, MockRenderable{ { 2, 0, 0 }, *tile_2_0_0 } },
                  { { 2, 0, 1 }, MockRenderable{ { 2, 0, 1 }, *tile_2_0_1 } },
                  { { 2, 1, 0 }, MockRenderable{ { 2, 1, 0 }, *tile_2_1_0 } },
              }),
              source.renderables);

    // Adding the last child tile covers 1/0/0 fully, so we don't need 0/0/0 anymore.
    auto tile_2_1_1 = source.createTileData(OverscaledTileID{ 2, 1, 1 });
    tile_2_1_1->ready = true;
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 2, 0, 0 }, MockRenderable{ { 2, 0, 0 }, *tile_2_0_0 } },
                  { { 2, 0, 1 }, MockRenderable{ { 2, 0, 1 }, *tile_2_0_1 } },
                  { { 2, 1, 0 }, MockRenderable{ { 2, 1, 0 }, *tile_2_1_0 } },
                  { { 2, 1, 1 }, MockRenderable{ { 2, 1, 1 }, *tile_2_1_1 } },
              }),
              source.renderables);
}

TEST(UpdateRenderables, UseParentAndChildTiles) {
    MockSource source;
    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 0 });

    auto tile_0_0_0 = source.createTileData(OverscaledTileID{ 0, 0, 0 });
    tile_0_0_0->ready = true;
    auto tile_2_0_0 = source.createTileData(OverscaledTileID{ 2, 0, 0 });
    tile_2_0_0->ready = true;

    // Check that it uses the child tile, but not the parent tile.
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 0, 0, 0 }, MockRenderable{ { 0, 0, 0 }, *tile_0_0_0 } },
                  { { 2, 0, 0 }, MockRenderable{ { 2, 0, 0 }, *tile_2_0_0 } },
              }),
              source.renderables);

    // Then, remove the child tile and check that it now uses the parent tile.
    source.dataTiles.erase(OverscaledTileID{ 2, 0, 0 });
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 1);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 0, 0, 0 }, MockRenderable{ { 0, 0, 0 }, *tile_0_0_0 } },
              }),
              source.renderables);
}

TEST(UpdateRenderables, DontUseTilesLowerThanMinzoom) {
    MockSource source;
    source.info.minZoom = 2;
    source.idealTiles.emplace(UnwrappedTileID{ 2, 0, 0 });

    auto tile_1_0_0 = source.createTileData(OverscaledTileID{ 1, 0, 0 });
    tile_1_0_0->ready = true;

    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 2);
    EXPECT_EQ(decltype(source.renderables)({}), source.renderables);
}

TEST(UpdateRenderables, UseOverzoomedTileAfterMaxzoom) {
    MockSource source;
    source.info.maxZoom = 2;
    source.idealTiles.emplace(UnwrappedTileID{ 2, 0, 0 });

    // Add a child tile (that should never occur in practice) and make sure it's not selected.
    auto tile_3_3_0_0 = source.createTileData(OverscaledTileID{ 3, 0, 0 });
    tile_3_3_0_0->ready = true;
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 2);
    EXPECT_EQ(decltype(source.renderables)({}), source.renderables);

    // Only add a non-overzoomed ("parent") tile at first.
    auto tile_2_2_0_0 = source.createTileData(OverscaledTileID{ 2, { 2, 0, 0 } });
    tile_2_2_0_0->ready = true;
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 3);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 2, 0, 0 }, MockRenderable{ { 2, 0, 0 }, *tile_2_2_0_0 } },
              }),
              source.renderables);

    // Then add the overzoomed tile matching the zoom level we're rendering.
    auto tile_3_2_0_0 = source.createTileData(OverscaledTileID{ 3, { 2, 0, 0 } });
    tile_3_2_0_0->ready = true;
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 3);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 2, 0, 0 }, MockRenderable{ { 2, 0, 0 }, *tile_3_2_0_0 } },
              }),
              source.renderables);

    // Check that it's switching back to the tile that has the matching overzoom value.
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 2);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 2, 0, 0 }, MockRenderable{ { 2, 0, 0 }, *tile_2_2_0_0 } },
              }),
              source.renderables);

    // Now remove the best match.
    source.dataTiles.erase(OverscaledTileID{ 2, { 2, 0, 0 } });
    tile_2_2_0_0 = nullptr;

    // Use the overzoomed tile even though it doesn't match the zoom level.
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 2);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 2, 0, 0 }, MockRenderable{ { 2, 0, 0 }, *tile_3_2_0_0 } },
              }),
              source.renderables);
}

TEST(UpdateRenderables, AscendToNonOverzoomedTiles) {
    MockSource source;
    source.info.maxZoom = 2;
    source.idealTiles.emplace(UnwrappedTileID{ 2, 0, 0 });

    // Add a matching overzoomed tile and make sure it gets selected.
    auto tile_3_2_0_0 = source.createTileData(OverscaledTileID{ 3, { 2, 0, 0 } });
    tile_3_2_0_0->ready = true;
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 3);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 2, 0, 0 }, MockRenderable{ { 2, 0, 0 }, *tile_3_2_0_0 } },
              }),
              source.renderables);

    // Then, swap it with a non-overzoomed tile.
    source.dataTiles.erase(OverscaledTileID{ 3, { 2, 0, 0 } });
    tile_3_2_0_0 = nullptr;
    auto tile_2_2_0_0 = source.createTileData(OverscaledTileID{ 2, { 2, 0, 0 } });
    tile_2_2_0_0->ready = true;
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 3);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 2, 0, 0 }, MockRenderable{ { 2, 0, 0 }, *tile_2_2_0_0 } },
              }),
              source.renderables);

    // Then, swap it with a parent tile.
    source.dataTiles.erase(OverscaledTileID{ 2, { 2, 0, 0 } });
    tile_2_2_0_0 = nullptr;
    auto tile_1_1_0_0 = source.createTileData(OverscaledTileID{ 1, { 1, 0, 0 } });
    tile_1_1_0_0->ready = true;
    source.renderables = algorithm::updateRenderables<MockRenderable>(
        source.dataTiles, source.idealTiles, source.info, 3);
    EXPECT_EQ(decltype(source.renderables)({
                  { { 1, 0, 0 }, MockRenderable{ { 1, 0, 0 }, *tile_1_1_0_0 } },
              }),
              source.renderables);
}
