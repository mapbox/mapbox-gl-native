#include "mock.hpp"
#include <mbgl/test/util.hpp>
#include <mbgl/util/variant.hpp>

#include <mapbox/variant_io.hpp>

#include <mbgl/algorithm/update_renderables.hpp>

using namespace mbgl;

enum LookupResult : bool {
    NotFound = false,
    Found = true,
};

struct GetTileDataAction {
    const OverscaledTileID tileID;
    const LookupResult found;

    bool operator==(const GetTileDataAction& rhs) const {
        return tileID == rhs.tileID && found == rhs.found;
    }
};

std::ostream& operator<<(std::ostream& os, const GetTileDataAction& action) {
    return os << "GetTileDataAction{ { " << int(action.tileID.overscaledZ) << ", " << int(action.tileID.wrap) << ", { "
              << int(action.tileID.canonical.z) << ", " << action.tileID.canonical.x << ", "
              << action.tileID.canonical.y << " } }, "
              << (action.found == Found ? "Found" : "NotFound") << " }\n";
}

struct CreateTileDataAction {
    const OverscaledTileID tileID;

    bool operator==(const CreateTileDataAction& rhs) const {
        return tileID == rhs.tileID;
    }
};

std::ostream& operator<<(std::ostream& os, const CreateTileDataAction& action) {
    return os << "CreateTileDataAction{ { " << int(action.tileID.overscaledZ) << ", " << int(action.tileID.wrap) << ", { "
              << int(action.tileID.canonical.z) << ", " << action.tileID.canonical.x << ", "
              << action.tileID.canonical.y << " } } }\n";
}

struct RetainTileDataAction {
    const OverscaledTileID tileID;
    const TileNecessity necessity;

    bool operator==(const RetainTileDataAction& rhs) const {
        return tileID == rhs.tileID && necessity == rhs.necessity;
    }
};

std::ostream& operator<<(std::ostream& os, const RetainTileDataAction& action) {
    return os << "RetainTileDataAction{ { " << int(action.tileID.overscaledZ) << ", " << int(action.tileID.wrap) << ", { "
              << int(action.tileID.canonical.z) << ", " << action.tileID.canonical.x << ", "
              << action.tileID.canonical.y << " } }, "
              << (action.necessity == TileNecessity::Required ? "Required" : "Optional") << " }\n";
}

struct RenderTileAction {
    const UnwrappedTileID tileID;
    const MockTileData& tileData;

    bool operator==(const RenderTileAction& rhs) const {
        return tileID == rhs.tileID && &tileData == &rhs.tileData;
    }
};

std::ostream& operator<<(std::ostream& os, const RenderTileAction& action) {
    const int64_t x =
        (1ul << action.tileID.canonical.z) * action.tileID.wrap + action.tileID.canonical.x;
    return os << "RenderTileAction{ { " << int(action.tileID.canonical.z) << ", " << x << ", "
              << action.tileID.canonical.y << " }, *tile_"
              << int(action.tileData.tileID.overscaledZ) << "_"
              << int(action.tileData.tileID.canonical.z) << "_"
              << action.tileData.tileID.canonical.x << "_" << action.tileData.tileID.canonical.y
              << " }\n";
}

using ActionLogEntry =
    variant<GetTileDataAction, CreateTileDataAction, RetainTileDataAction, RenderTileAction>;
using ActionLog = std::vector<ActionLogEntry>;

template <typename T>
auto getTileDataFn(ActionLog& log, const T& dataTiles) {
    return [&](const auto& id) {
        auto it = dataTiles.find(id);
        log.emplace_back(GetTileDataAction{ id, it != dataTiles.end() ? Found : NotFound });
        return (it != dataTiles.end()) ? it->second.get() : nullptr;
    };
}

template <typename T>
auto createTileDataFn(ActionLog& log, T& dataTiles) {
    return [&](const auto& id) {
        log.emplace_back(CreateTileDataAction{ id });
        return (dataTiles[id] = std::make_unique<MockTileData>(id)).get();
    };
}

template <typename = int>
auto retainTileDataFn(ActionLog& log) {
    return [&](auto& tileData, TileNecessity necessity) {
        log.emplace_back(RetainTileDataAction{ tileData.tileID, necessity });
    };
}

template <typename = int>
auto renderTileFn(ActionLog& log) {
    return [&](const auto& id, auto& tileData) {
        log.emplace_back(RenderTileAction{ id, tileData });
    };
}

TEST(UpdateRenderables, SingleTile) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.idealTiles.emplace(UnwrappedTileID{ 1, 1, 1 });

    // Make sure that we're getting the tile back.
    auto tile_1_1_1_1 = source.createTileData(OverscaledTileID{ 1, 1, 1 });
    tile_1_1_1_1->renderable = true;

    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 1, 1 } }, Found },       // found ideal tile
                  RetainTileDataAction{ { 1, 0, { 1, 1, 1 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 1, 1, 1 }, *tile_1_1_1_1 },       // render ideal tile
              }),
              log);

    // Check a repeated render with the same data.
    log.clear();
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 1, 1 } }, Found },       // found ideal tile
                  RetainTileDataAction{ { 1, 0, { 1, 1, 1 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 1, 1, 1 }, *tile_1_1_1_1 },       // render ideal tile
              }),
              log);

    // Insert a tile we don't have data for.
    log.clear();
    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 1 });
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 0, 1 } }, NotFound },    // missing ideal tile
                  CreateTileDataAction{ { 1, 0, { 1, 0, 1 } } },           // create ideal tile
                  RetainTileDataAction{ { 1, 0, { 1, 0, 1 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 0, 2 } }, NotFound },    // four child tiles
                  GetTileDataAction{ { 2, 0, { 2, 0, 3 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 1, 2 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 1, 3 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    // parent tile

                  GetTileDataAction{ { 1, 0, { 1, 1, 1 } }, Found },       // found ideal tile
                  RetainTileDataAction{ { 1, 0, { 1, 1, 1 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 1, 1, 1 }, *tile_1_1_1_1 },       // render found tile
              }),
              log);

    // Mark the created tile as having the optional request tried.
    log.clear();
    source.dataTiles[{ 1, 0, { 1, 0, 1 } }]->triedOptional = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 0, 1 } }, Found },       // missing ideal tile
                  RetainTileDataAction{ { 1, 0, { 1, 0, 1 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 0, 2 } }, NotFound },    // four child tiles
                  GetTileDataAction{ { 2, 0, { 2, 0, 3 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 1, 2 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 1, 3 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    // parent tile
                  CreateTileDataAction{ { 0, 0, { 0, 0, 0 } } },           // load parent tile
                  RetainTileDataAction{ { 0, 0, { 0, 0, 0 } }, TileNecessity::Optional }, //

                  GetTileDataAction{ { 1, 0, { 1, 1, 1 } }, Found },       // found ideal tile
                  RetainTileDataAction{ { 1, 0, { 1, 1, 1 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 1, 1, 1 }, *tile_1_1_1_1 },       // render found tile
              }),
              log);

    // Now insert the missing tile and check that we're rendering it.
    log.clear();
    auto tile_1_1_0_1 = source.createTileData(OverscaledTileID{ 1, 0, 1 });
    tile_1_1_0_1->renderable = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 0, 1 } }, Found },       // newly added tile
                  RetainTileDataAction{ { 1, 0, { 1, 0, 1 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 1, 0, 1 }, *tile_1_1_0_1 },       // render ideal tile

                  GetTileDataAction{ { 1, 0, { 1, 1, 1 } }, Found },       // ideal tile
                  RetainTileDataAction{ { 1, 0, { 1, 1, 1 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 1, 1, 1 }, *tile_1_1_1_1 },       // render found tile
              }),
              log);

    // Insert another tile, and another bucket that has a different name and check that we're not
    // using it.
    log.clear();
    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 0 });
    auto tile_1_1_0_0 = source.createTileData(OverscaledTileID{ 1, 0, 0 });

    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, Found },       // found tile, not ready
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    // four child tiles
                  GetTileDataAction{ { 2, 0, { 2, 0, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, Found },       // parent tile
                  RetainTileDataAction{ { 0, 0, { 0, 0, 0 } }, TileNecessity::Optional }, //
                  // optional parent tile was already created before, but is not renderable

                  GetTileDataAction{ { 1, 0, { 1, 0, 1 } }, Found },       // ideal tile
                  RetainTileDataAction{ { 1, 0, { 1, 0, 1 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 1, 0, 1 }, *tile_1_1_0_1 },       // render ideal tile

                  GetTileDataAction{ { 1, 0, { 1, 1, 1 } }, Found },       // ideal tile
                  RetainTileDataAction{ { 1, 0, { 1, 1, 1 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 1, 1, 1 }, *tile_1_1_1_1 },       // render ideal tile
              }),
              log);

    // Then, add the bucket and check that it's getting used.
    log.clear();
    tile_1_1_0_0->renderable = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, Found },       // found tile, now ready
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 1, 0, 0 }, *tile_1_1_0_0 },       //

                  GetTileDataAction{ { 1, 0, { 1, 0, 1 } }, Found },       // ideal tile
                  RetainTileDataAction{ { 1, 0, { 1, 0, 1 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 1, 0, 1 }, *tile_1_1_0_1 },       //

                  GetTileDataAction{ { 1, 0, { 1, 1, 1 } }, Found },       // ideal tile
                  RetainTileDataAction{ { 1, 0, { 1, 1, 1 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 1, 1, 1 }, *tile_1_1_1_1 },       //
              }),
              log);
}

TEST(UpdateRenderables, UseParentTile) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 1 });
    source.idealTiles.emplace(UnwrappedTileID{ 1, 1, 0 });
    source.idealTiles.emplace(UnwrappedTileID{ 1, 1, 1 });

    // Make sure that we're getting the tile back.
    auto tile_0_0_0_0 = source.createTileData(OverscaledTileID{ 0, 0, 0 });
    tile_0_0_0_0->renderable = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 0, 1 } }, NotFound },    // missing ideal tile
                  CreateTileDataAction{ { 1, 0, { 1, 0, 1 } } },           //
                  RetainTileDataAction{ { 1, 0, { 1, 0, 1 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 0, 2 } }, NotFound },    // child tile
                  GetTileDataAction{ { 2, 0, { 2, 0, 3 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 1, 2 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 1, 3 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, Found },       // parent found!
                  RetainTileDataAction{ { 0, 0, { 0, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 0, 0, 0 }, *tile_0_0_0_0 },       // render parent
                  GetTileDataAction{ { 1, 0, { 1, 1, 0 } }, NotFound },    // missing ideal tile
                  CreateTileDataAction{ { 1, 0, { 1, 1, 0 } } },           //
                  RetainTileDataAction{ { 1, 0, { 1, 1, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 2, 0 } }, NotFound },    // child tile
                  GetTileDataAction{ { 2, 0, { 2, 2, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 3, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 3, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 1, 0, { 1, 1, 1 } }, NotFound },    // missing tile
                  CreateTileDataAction{ { 1, 0, { 1, 1, 1 } } },           //
                  RetainTileDataAction{ { 1, 0, { 1, 1, 1 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 2, 2 } }, NotFound },    // child tile
                  GetTileDataAction{ { 2, 0, { 2, 2, 3 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 3, 2 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 3, 3 } }, NotFound },    // ...
              }),
              log);
}

TEST(UpdateRenderables, DontUseWrongParentTile) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.idealTiles.emplace(UnwrappedTileID{ 2, 0, 0 });

    auto tile_1_1_1_0 = source.createTileData(OverscaledTileID{ 1, 1, 0 });
    tile_1_1_1_0->renderable = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 2);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    // missing ideal tile
                  CreateTileDataAction{ { 2, 0, { 2, 0, 0 } } },           //
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 3, 0, { 3, 0, 0 } }, NotFound },    // child tile
                  GetTileDataAction{ { 3, 0, { 3, 0, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 3, 0, { 3, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 3, 0, { 3, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    // parent tile, missing
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    // parent tile, missing
              }),
              log);

    // Now mark the created tile as having the optional request tried.
    log.clear();
    source.dataTiles[{ 2, 0, { 2, 0, 0 } }]->triedOptional = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 2);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, Found },       // non-ready ideal tile
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 3, 0, { 3, 0, 0 } }, NotFound },    // child tile
                  GetTileDataAction{ { 3, 0, { 3, 0, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 3, 0, { 3, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 3, 0, { 3, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    // parent tile, missing
                  CreateTileDataAction{ { 1, 0, { 1, 0, 0 } } },           // find optional parent
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    // parent tile, missing
              }),
              log);

    // Add a new child tile and check that it is now used.
    log.clear();
    source.idealTiles.emplace(UnwrappedTileID{ 2, 2, 0 });
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 2);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, Found },       // non-ready ideal tile
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Required }, //
                  // this tile was added by the previous invocation of updateRenderables
                  GetTileDataAction{ { 3, 0, { 3, 0, 0 } }, NotFound },    // child tile
                  GetTileDataAction{ { 3, 0, { 3, 0, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 3, 0, { 3, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 3, 0, { 3, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, Found },       // parent tile not ready
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    // missing parent tile

                  GetTileDataAction{ { 2, 0, { 2, 2, 0 } }, NotFound },    // missing ideal tile
                  CreateTileDataAction{ { 2, 0, { 2, 2, 0 } } },           //
                  RetainTileDataAction{ { 2, 0, { 2, 2, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 3, 0, { 3, 4, 0 } }, NotFound },    // child tile
                  GetTileDataAction{ { 3, 0, { 3, 4, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 3, 0, { 3, 5, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 3, 0, { 3, 5, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 1, 0, { 1, 1, 0 } }, Found },       // found parent tile
                  RetainTileDataAction{ { 1, 0, { 1, 1, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 1, 1, 0 }, *tile_1_1_1_0 },       // render parent tile
              }),
              log);
}

TEST(UpdateRenderables, UseParentTileWhenChildNotReady) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 1 });

    auto tile_0_0_0_0 = source.createTileData(OverscaledTileID{ 0, 0, 0 });
    tile_0_0_0_0->renderable = true;

    auto tile_1_1_0_1 = source.createTileData(OverscaledTileID{ 1, 0, 1 });
    // Don't create bucket.

    // Make sure that it renders the parent tile.
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 0, 1 } }, Found },       // found, but not ready
                  RetainTileDataAction{ { 1, 0, { 1, 0, 1 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 0, 2 } }, NotFound },    // child tile
                  GetTileDataAction{ { 2, 0, { 2, 0, 3 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 1, 2 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 1, 3 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, Found },       // parent tile, ready
                  RetainTileDataAction{ { 0, 0, { 0, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 0, 0, 0 }, *tile_0_0_0_0 },       // render parent tile
              }),
              log);

    // Now insert the bucket and make sure we're now using the matching tile
    log.clear();
    tile_1_1_0_1->renderable = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 0, 1 } }, Found },       // found and ready
                  RetainTileDataAction{ { 1, 0, { 1, 0, 1 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 1, 0, 1 }, *tile_1_1_0_1 },       // render ideal tile
              }),
              log);
}

TEST(UpdateRenderables, UseOverlappingParentTile) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 0 });
    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 1 });

    auto tile_0_0_0_0 = source.createTileData(OverscaledTileID{ 0, 0, 0 });
    tile_0_0_0_0->renderable = true;

    auto tile_1_1_0_1 = source.createTileData(OverscaledTileID{ 1, 0, 1 });
    tile_1_1_0_1->renderable = true;

    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    // ideal tile not found
                  CreateTileDataAction{ { 1, 0, { 1, 0, 0 } } },           //
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    // child tile
                  GetTileDataAction{ { 2, 0, { 2, 0, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, Found },       // parent tile found
                  RetainTileDataAction{ { 0, 0, { 0, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 0, 0, 0 }, *tile_0_0_0_0 },       //

                  GetTileDataAction{ { 1, 0, { 1, 0, 1 } }, Found },       // ideal tile found
                  RetainTileDataAction{ { 1, 0, { 1, 0, 1 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 1, 0, 1 }, *tile_1_1_0_1 },       //
              }),
              log);
}

TEST(UpdateRenderables, UseChildTiles) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.idealTiles.emplace(UnwrappedTileID{ 0, 0, 0 });

    auto tile_1_1_0_0 = source.createTileData(OverscaledTileID{ 1, 0, 0 });
    tile_1_1_0_0->renderable = true;
    auto tile_1_1_1_0 = source.createTileData(OverscaledTileID{ 1, 1, 0 });
    tile_1_1_1_0->renderable = true;

    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 0);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    // ideal tile, missing
                  CreateTileDataAction{ { 0, 0, { 0, 0, 0 } } },           //
                  RetainTileDataAction{ { 0, 0, { 0, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, Found },       // child tile found
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 1, 0, 0 }, *tile_1_1_0_0 },       // render child tile
                  GetTileDataAction{ { 1, 0, { 1, 0, 1 } }, NotFound },    // child tile not found
                  GetTileDataAction{ { 1, 0, { 1, 1, 0 } }, Found },       // child tile found
                  RetainTileDataAction{ { 1, 0, { 1, 1, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 1, 1, 0 }, *tile_1_1_1_0 },       // render child tile
                  GetTileDataAction{ { 1, 0, { 1, 1, 1 } }, NotFound },    // child tile not found
                  // no parent tile of 0 to consider
              }),
              log);
}

TEST(UpdateRenderables, PreferChildTiles) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 0 });

    auto tile_0_0_0_0 = source.createTileData(OverscaledTileID{ 0, 0, 0 });
    tile_0_0_0_0->renderable = true;
    auto tile_2_2_0_0 = source.createTileData(OverscaledTileID{ 2, 0, 0 });
    tile_2_2_0_0->renderable = true;

    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    // ideal tile, not found
                  CreateTileDataAction{ { 1, 0, { 1, 0, 0 } } },           //
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, Found },       // child tile, found
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 2, 0, 0 }, *tile_2_2_0_0 },       //
                  GetTileDataAction{ { 2, 0, { 2, 0, 1 } }, NotFound },    // child tile, not found
                  GetTileDataAction{ { 2, 0, { 2, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, Found },       // parent tile, found
                  RetainTileDataAction{ { 0, 0, { 0, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 0, 0, 0 }, *tile_0_0_0_0 },       //
              }),
              log);

    // Now add more children to cover the ideal tile fully, until it is covered fully, and verify
    // that the parent doesn't get rendered.
    log.clear();
    auto tile_2_2_0_1 = source.createTileData(OverscaledTileID{ 2, 0, 1 });
    tile_2_2_0_1->renderable = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, Found }, // ideal tile, not ready
                  // ideal tile was added in previous invocation, but is not yet ready
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, Found },       // child tile, found
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 2, 0, 0 }, *tile_2_2_0_0 },       //
                  GetTileDataAction{ { 2, 0, { 2, 0, 1 } }, Found },       // ...
                  RetainTileDataAction{ { 2, 0, { 2, 0, 1 } }, TileNecessity::Optional }, // ...
                  RenderTileAction{ { 2, 0, 1 }, *tile_2_2_0_1 },       //
                  GetTileDataAction{ { 2, 0, { 2, 1, 0 } }, NotFound },    // child tile, not found
                  GetTileDataAction{ { 2, 0, { 2, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, Found },       // parent tile, found
                  RetainTileDataAction{ { 0, 0, { 0, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 0, 0, 0 }, *tile_0_0_0_0 },       //
              }),
              log);

    log.clear();
    auto tile_2_2_1_0 = source.createTileData(OverscaledTileID{ 2, 1, 0 });
    tile_2_2_1_0->renderable = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, Found }, // ideal tile, not ready
                  // ideal tile was added in first invocation, but is not yet ready
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, Found },       // child tile, found
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 2, 0, 0 }, *tile_2_2_0_0 },       //
                  GetTileDataAction{ { 2, 0, { 2, 0, 1 } }, Found },       // ...
                  RetainTileDataAction{ { 2, 0, { 2, 0, 1 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 2, 0, 1 }, *tile_2_2_0_1 },       //
                  GetTileDataAction{ { 2, 0, { 2, 1, 0 } }, Found },       // ...
                  RetainTileDataAction{ { 2, 0, { 2, 1, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 2, 1, 0 }, *tile_2_2_1_0 },       //
                  GetTileDataAction{ { 2, 0, { 2, 1, 1 } }, NotFound },    // child tile, not found
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, Found },       // parent tile, found
                  RetainTileDataAction{ { 0, 0, { 0, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 0, 0, 0 }, *tile_0_0_0_0 },       //
              }),
              log);

    // Adding the last child tile covers 1/0/0 fully, so we don't need 0/0/0 anymore.
    log.clear();
    auto tile_2_2_1_1 = source.createTileData(OverscaledTileID{ 2, 1, 1 });
    tile_2_2_1_1->renderable = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, Found }, // ideal tile, not ready
                  // ideal tile was added in first invocation, but is not yet ready
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, Found },       // child tile, found
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 2, 0, 0 }, *tile_2_2_0_0 },       //
                  GetTileDataAction{ { 2, 0, { 2, 0, 1 } }, Found },       // ...
                  RetainTileDataAction{ { 2, 0, { 2, 0, 1 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 2, 0, 1 }, *tile_2_2_0_1 },       //
                  GetTileDataAction{ { 2, 0, { 2, 1, 0 } }, Found },       // ...
                  RetainTileDataAction{ { 2, 0, { 2, 1, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 2, 1, 0 }, *tile_2_2_1_0 },       //
                  GetTileDataAction{ { 2, 0, { 2, 1, 1 } }, Found },       // ...
                  RetainTileDataAction{ { 2, 0, { 2, 1, 1 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 2, 1, 1 }, *tile_2_2_1_1 },       //
              }),
              log);
}

TEST(UpdateRenderables, UseParentAndChildTiles) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 0 });

    auto tile_0_0_0_0 = source.createTileData(OverscaledTileID{ 0, 0, 0 });
    tile_0_0_0_0->renderable = true;
    auto tile_2_2_0_0 = source.createTileData(OverscaledTileID{ 2, 0, 0 });
    tile_2_2_0_0->renderable = true;

    // Check that it uses the child tile and the parent tile to cover the rest.
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    // ideal tile, missing
                  CreateTileDataAction{ { 1, 0, { 1, 0, 0 } } },           //
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, Found },       // child tile
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 2, 0, 0 }, *tile_2_2_0_0 },       //
                  GetTileDataAction{ { 2, 0, { 2, 0, 1 } }, NotFound },    //
                  GetTileDataAction{ { 2, 0, { 2, 1, 0 } }, NotFound },    //
                  GetTileDataAction{ { 2, 0, { 2, 1, 1 } }, NotFound },    //
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, Found },       // parent tile
                  RetainTileDataAction{ { 0, 0, { 0, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 0, 0, 0 }, *tile_0_0_0_0 },       //
              }),
              log);

    // Then, remove the child tile and check that it now only the parent tile.
    log.clear();
    source.dataTiles.erase(OverscaledTileID{ 2, 0, 0 });
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, Found },       // ideal tile, not ready
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    //
                  GetTileDataAction{ { 2, 0, { 2, 0, 1 } }, NotFound },    //
                  GetTileDataAction{ { 2, 0, { 2, 1, 0 } }, NotFound },    //
                  GetTileDataAction{ { 2, 0, { 2, 1, 1 } }, NotFound },    //
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, Found },       // parent tile
                  RetainTileDataAction{ { 0, 0, { 0, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 0, 0, 0 }, *tile_0_0_0_0 },       //
              }),
              log);
}

TEST(UpdateRenderables, DontUseTilesLowerThanMinzoom) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.zoomRange.min = 2;
    source.idealTiles.emplace(UnwrappedTileID{ 2, 0, 0 });

    auto tile_1_1_0_0 = source.createTileData(OverscaledTileID{ 1, 0, 0 });
    tile_1_1_0_0->renderable = true;

    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 2);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    // ideal tile, missing
                  CreateTileDataAction{ { 2, 0, { 2, 0, 0 } } },           //
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 3, 0, { 3, 0, 0 } }, NotFound },    //
                  GetTileDataAction{ { 3, 0, { 3, 0, 1 } }, NotFound },    //
                  GetTileDataAction{ { 3, 0, { 3, 1, 0 } }, NotFound },    //
                  GetTileDataAction{ { 3, 0, { 3, 1, 1 } }, NotFound },    //
                  // no requests for zoom 1 tiles
              }),
              log);
}

TEST(UpdateRenderables, UseOverzoomedTileAfterMaxzoom) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.zoomRange.max = 2;
    source.idealTiles.emplace(UnwrappedTileID{ 2, 0, 0 });

    // Add a child tile (that should never occur in practice) and make sure it's not selected.
    auto tile_3_3_0_0 = source.createTileData(OverscaledTileID{ 3, 0, 0 });
    tile_3_3_0_0->renderable = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 2);
    EXPECT_EQ(
        ActionLog({
            GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    // ideal tile, missing
            CreateTileDataAction{ { 2, 0, { 2, 0, 0 } } },           //
            RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Required }, //
            GetTileDataAction{ { 3, 0, { 2, 0, 0 } }, NotFound },    // overzoomed tile, not children!
            GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    //
            GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    //
        }),
        log);

    // Mark the created tile as having tried the optional request.
    log.clear();
    source.dataTiles[{ 2, 0, { 2, 0, 0 } }]->triedOptional = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 2);
    EXPECT_EQ(
        ActionLog({
            GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, Found },       // ideal tile, missing
            RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Required }, //
            GetTileDataAction{ { 3, 0, { 2, 0, 0 } }, NotFound },    // overzoomed tile, not children!
            GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    //
            CreateTileDataAction{ { 1, 0, { 1, 0, 0 } } },           //
            RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Optional }, //
            GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    //
        }),
        log);

    // Only add a non-overzoomed ("parent") tile at first.
    log.clear();
    auto tile_2_2_0_0 = source.createTileData(OverscaledTileID{ 2, 0, { 2, 0, 0 } });
    tile_2_2_0_0->renderable = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 3);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 3, 0, { 2, 0, 0 } }, NotFound },    // ideal tile, missing
                  CreateTileDataAction{ { 3, 0, { 2, 0, 0 } } },           //
                  RetainTileDataAction{ { 3, 0, { 2, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 4, 0, { 2, 0, 0 } }, NotFound },    //
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, Found },       //
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 2, 0, 0 }, *tile_2_2_0_0 },       //
              }),
              log);

    // Then add the overzoomed tile matching the zoom level we're rendering.
    log.clear();
    auto tile_3_2_0_0 = source.createTileData(OverscaledTileID{ 3, 0, { 2, 0, 0 } });
    tile_3_2_0_0->renderable = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 3);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 3, 0, { 2, 0, 0 } }, Found },       //
                  RetainTileDataAction{ { 3, 0, { 2, 0, 0 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 2, 0, 0 }, *tile_3_2_0_0 },       //
              }),
              log);

    // Check that it's switching back to the tile that has the matching overzoom value.
    log.clear();
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 2);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, Found },       //
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 2, 0, 0 }, *tile_2_2_0_0 },       //
              }),
              log);

    // Now remove the best match.
    log.clear();
    source.dataTiles.erase(OverscaledTileID{ 2, 0, { 2, 0, 0 } });
    tile_2_2_0_0 = nullptr;

    // Use the overzoomed tile even though it doesn't match the zoom level.
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 2);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    //
                  CreateTileDataAction{ { 2, 0, { 2, 0, 0 } } },           //
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 3, 0, { 2, 0, 0 } }, Found },       // use overzoomed tile!
                  RetainTileDataAction{ { 3, 0, { 2, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 2, 0, 0 }, *tile_3_2_0_0 },       //
              }),
              log);
}

TEST(UpdateRenderables, AscendToNonOverzoomedTiles) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.zoomRange.max = 2;
    source.idealTiles.emplace(UnwrappedTileID{ 2, 0, 0 });

    // Add a matching overzoomed tile and make sure it gets selected.
    auto tile_3_2_0_0 = source.createTileData(OverscaledTileID{ 3, 0, { 2, 0, 0 } });
    tile_3_2_0_0->renderable = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 3);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 3, 0, { 2, 0, 0 } }, Found },       //
                  RetainTileDataAction{ { 3, 0, { 2, 0, 0 } }, TileNecessity::Required }, //
                  RenderTileAction{ { 2, 0, 0 }, *tile_3_2_0_0 },       //
              }),
              log);

    // Then, swap it with a non-overzoomed tile.
    log.clear();
    source.dataTiles.erase(OverscaledTileID{ 3, 0, { 2, 0, 0 } });
    tile_3_2_0_0 = nullptr;
    auto tile_2_2_0_0 = source.createTileData(OverscaledTileID{ 2, 0, { 2, 0, 0 } });
    tile_2_2_0_0->renderable = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 3);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 3, 0, { 2, 0, 0 } }, NotFound },    //
                  CreateTileDataAction{ { 3, 0, { 2, 0, 0 } } },           //
                  RetainTileDataAction{ { 3, 0, { 2, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 4, 0, { 2, 0, 0 } }, NotFound }, // prefer using a child first
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, Found },    //
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 2, 0, 0 }, *tile_2_2_0_0 },       //
              }),
              log);

    // Then, swap it with a parent tile.
    log.clear();
    source.dataTiles.erase(OverscaledTileID{ 2, 0, { 2, 0, 0 } });
    tile_2_2_0_0 = nullptr;
    auto tile_1_1_0_0 = source.createTileData(OverscaledTileID{ 1, 0, { 1, 0, 0 } });
    tile_1_1_0_0->renderable = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 3);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 3, 0, { 2, 0, 0 } }, Found },       // ideal tile, not ready
                  RetainTileDataAction{ { 3, 0, { 2, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 4, 0, { 2, 0, 0 } }, NotFound },    //
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    //
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, Found },       //
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 1, 0, 0 }, *tile_1_1_0_0 },       //
              }),
              log);

    // Now, mark the created tile as found.
    log.clear();
    source.dataTiles[{ 3, 0, { 2, 0, 0 } }]->triedOptional = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 3);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 3, 0, { 2, 0, 0 } }, Found },       // ideal tile, not ready
                  RetainTileDataAction{ { 3, 0, { 2, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 4, 0, { 2, 0, 0 } }, NotFound },    //
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    //
                  CreateTileDataAction{ { 2, 0, { 2, 0, 0 } } },           //
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, Found },       //
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 1, 0, 0 }, *tile_1_1_0_0 },       //
              }),
              log);
}

TEST(UpdateRenderables, DoNotAscendMultipleTimesIfNotFound) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.idealTiles.emplace(UnwrappedTileID{ 8, 0, 0 });
    source.idealTiles.emplace(UnwrappedTileID{ 8, 1, 0 });

    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 8);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 8, 0, { 8, 0, 0 } }, NotFound },    // ideal tile
                  CreateTileDataAction{ { 8, 0, { 8, 0, 0 } } },           //
                  RetainTileDataAction{ { 8, 0, { 8, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 9, 0, { 9, 0, 0 } }, NotFound },    // child tile
                  GetTileDataAction{ { 9, 0, { 9, 0, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 9, 0, { 9, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 9, 0, { 9, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 0, 0 } }, NotFound },    // ascent
                  GetTileDataAction{ { 6, 0, { 6, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 5, 0, { 5, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 4, 0, { 4, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 3, 0, { 3, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    // ...

                  GetTileDataAction{ { 8, 0, { 8, 1, 0 } }, NotFound },    // ideal tile
                  CreateTileDataAction{ { 8, 0, { 8, 1, 0 } } },           //
                  RetainTileDataAction{ { 8, 0, { 8, 1, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 9, 0, { 9, 2, 0 } }, NotFound },    // child tile
                  GetTileDataAction{ { 9, 0, { 9, 2, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 9, 0, { 9, 3, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 9, 0, { 9, 3, 1 } }, NotFound },    // ...
                  // no second ascent to 0
              }),
              log);

    // Now add a mid-level tile that stops the ascent
    log.clear();
    auto tile_4_0_0_0 = source.createTileData(OverscaledTileID{ 4, 0, { 4, 0, 0 } });
    tile_4_0_0_0->renderable = true;

    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 8);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 8, 0, { 8, 0, 0 } }, Found },       // ideal tile, not ready
                  RetainTileDataAction{ { 8, 0, { 8, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 9, 0, { 9, 0, 0 } }, NotFound },    // child tile
                  GetTileDataAction{ { 9, 0, { 9, 0, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 9, 0, { 9, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 9, 0, { 9, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 0, 0 } }, NotFound },    // ascent
                  GetTileDataAction{ { 6, 0, { 6, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 5, 0, { 5, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 4, 0, { 4, 0, 0 } }, Found },       // stops ascent
                  RetainTileDataAction{ { 4, 0, { 4, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 4, 0, 0 }, *tile_4_0_0_0 },       //

                  GetTileDataAction{ { 8, 0, { 8, 1, 0 } }, Found },       // ideal tile, not ready
                  RetainTileDataAction{ { 8, 0, { 8, 1, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 9, 0, { 9, 2, 0 } }, NotFound },    // child tile
                  GetTileDataAction{ { 9, 0, { 9, 2, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 9, 0, { 9, 3, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 9, 0, { 9, 3, 1 } }, NotFound },    // ...
                  // no second ascent to 0
              }),
              log);
}

TEST(UpdateRenderables, DontRetainUnusedNonIdealTiles) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.idealTiles.emplace(UnwrappedTileID{ 2, 0, 0 });

    source.createTileData(OverscaledTileID{ 1, 0, 0 });
    source.createTileData(OverscaledTileID{ 2, 0, 0 });

    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 2);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, Found },       // ideal tile, not ready
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 3, 0, { 3, 0, 0 } }, NotFound },    //
                  GetTileDataAction{ { 3, 0, { 3, 0, 1 } }, NotFound },    //
                  GetTileDataAction{ { 3, 0, { 3, 1, 0 } }, NotFound },    //
                  GetTileDataAction{ { 3, 0, { 3, 1, 1 } }, NotFound },    //
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, Found },       // parent tile, not ready
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    //
              }),
              log);
}

TEST(UpdateRenderables, WrappedTiles) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.idealTiles.emplace(UnwrappedTileID{ 1, -1, 0 }); // 'wrap' -> -1
    source.idealTiles.emplace(UnwrappedTileID{ 1, 0, 0 });  // 'wrap' -> 0
    source.idealTiles.emplace(UnwrappedTileID{ 1, 1, 0 });  // 'wrap' -> 0
    source.idealTiles.emplace(UnwrappedTileID{ 1, 2, 0 });  // 'wrap' -> 1

    auto tile_0_0_0_0 = source.createTileData(OverscaledTileID{ 0, 0, { 0, 0, 0 } });
    tile_0_0_0_0->renderable = true;

    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 1);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 1, -1, { 1, 1, 0 } }, NotFound },    // ideal tile 1/-1/0 (wrapped to -1)
                  CreateTileDataAction{ { 1, -1, { 1, 1, 0 } } },           //
                  RetainTileDataAction{ { 1, -1, { 1, 1, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, -1, { 2, 2, 0 } }, NotFound },    //
                  GetTileDataAction{ { 2, -1, { 2, 2, 1 } }, NotFound },    //
                  GetTileDataAction{ { 2, -1, { 2, 3, 0 } }, NotFound },    //
                  GetTileDataAction{ { 2, -1, { 2, 3, 1 } }, NotFound },    //
                  GetTileDataAction{ { 0, -1, { 0, 0, 0 } }, NotFound },    // { 0, 0, 0 } exists, but not the version wrapped to -1

                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    // ideal tile 1/0/0
                  CreateTileDataAction{ { 1, 0, { 1, 0, 0 } } },           //
                  RetainTileDataAction{ { 1, 0, { 1, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    //
                  GetTileDataAction{ { 2, 0, { 2, 0, 1 } }, NotFound },    //
                  GetTileDataAction{ { 2, 0, { 2, 1, 0 } }, NotFound },    //
                  GetTileDataAction{ { 2, 0, { 2, 1, 1 } }, NotFound },    //
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, Found },       //
                  RetainTileDataAction{ { 0, 0, { 0, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 0, 0, 0 }, *tile_0_0_0_0 },       //

                  GetTileDataAction{ { 1, 0, { 1, 1, 0 } }, NotFound },       // ideal tile 1/1/0, doesn't match 1/-/1/0
                  CreateTileDataAction{ { 1, 0, { 1, 1, 0 } } },
                  RetainTileDataAction{ { 1, 0, { 1, 1, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 0, { 2, 2, 0 } }, NotFound },    //
                  GetTileDataAction{ { 2, 0, { 2, 2, 1 } }, NotFound },    //
                  GetTileDataAction{ { 2, 0, { 2, 3, 0 } }, NotFound },    //
                  GetTileDataAction{ { 2, 0, { 2, 3, 1 } }, NotFound },    //
                  // do not ascent; 0/0/0 has been rendered already for 1/0/0

                  GetTileDataAction{ { 1, 1, { 1, 0, 0 } }, NotFound },       // ideal tile 1/2/0 (wrapped to 1)
                  CreateTileDataAction{ { 1, 1, { 1, 0, 0 } } },
                  RetainTileDataAction{ { 1, 1, { 1, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 2, 1, { 2, 0, 0 } }, NotFound },    //
                  GetTileDataAction{ { 2, 1, { 2, 0, 1 } }, NotFound },    //
                  GetTileDataAction{ { 2, 1, { 2, 1, 0 } }, NotFound },    //
                  GetTileDataAction{ { 2, 1, { 2, 1, 1 } }, NotFound },    //
                  GetTileDataAction{ { 0, 1, { 0, 0, 0 } }, NotFound },    // { 0, 0, 0 } exists, but not the version wrapped to -1
              }),
              log);
}

TEST(UpdateRenderables, RepeatedRenderWithMissingOptionals) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.idealTiles.emplace(UnwrappedTileID{ 6, 0, 0 });

    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 6);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 6, 0, { 6, 0, 0 } }, NotFound },    // ideal tile, not found
                  CreateTileDataAction{ { 6, 0, { 6, 0, 0 } } },           //
                  RetainTileDataAction{ { 6, 0, { 6, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 7, 0, { 7, 0, 0 } }, NotFound },    // children
                  GetTileDataAction{ { 7, 0, { 7, 0, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 5, 0, { 5, 0, 0 } }, NotFound },    // ascent
                  GetTileDataAction{ { 4, 0, { 4, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 3, 0, { 3, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    // ...
              }),
              log);

    // Repeat.
    log.clear();
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 6);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 6, 0, { 6, 0, 0 } }, Found },       // ideal tile, not ready
                  RetainTileDataAction{ { 6, 0, { 6, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 7, 0, { 7, 0, 0 } }, NotFound },    // children
                  GetTileDataAction{ { 7, 0, { 7, 0, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 5, 0, { 5, 0, 0 } }, NotFound },    // ascent
                  GetTileDataAction{ { 4, 0, { 4, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 3, 0, { 3, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    // ...
              }),
              log);

    // Mark next level has having tried optional.
    log.clear();
    source.dataTiles[{ 6, 0, { 6, 0, 0 } }]->triedOptional = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 6);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 6, 0, { 6, 0, 0 } }, Found },       // ideal tile, not ready
                  RetainTileDataAction{ { 6, 0, { 6, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 7, 0, { 7, 0, 0 } }, NotFound },    // children
                  GetTileDataAction{ { 7, 0, { 7, 0, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 5, 0, { 5, 0, 0 } }, NotFound },    // ascent
                  CreateTileDataAction{ { 5, 0, { 5, 0, 0 } } },           //
                  RetainTileDataAction{ { 5, 0, { 5, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 4, 0, { 4, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 3, 0, { 3, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    // ...
              }),
              log);

    // Repeat.
    log.clear();
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 6);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 6, 0, { 6, 0, 0 } }, Found },       // ideal tile, not ready
                  RetainTileDataAction{ { 6, 0, { 6, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 7, 0, { 7, 0, 0 } }, NotFound },    // children
                  GetTileDataAction{ { 7, 0, { 7, 0, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 5, 0, { 5, 0, 0 } }, Found },       // ascent
                  RetainTileDataAction{ { 5, 0, { 5, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 4, 0, { 4, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 3, 0, { 3, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    // ...
              }),
              log);

    // Mark next level has having tried optional.
    log.clear();
    source.dataTiles[{ 5, 0, { 5, 0, 0 } }]->triedOptional = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 6);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 6, 0, { 6, 0, 0 } }, Found },       // ideal tile, not ready
                  RetainTileDataAction{ { 6, 0, { 6, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 7, 0, { 7, 0, 0 } }, NotFound },    // children
                  GetTileDataAction{ { 7, 0, { 7, 0, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 5, 0, { 5, 0, 0 } }, Found },       // ascent
                  RetainTileDataAction{ { 5, 0, { 5, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 4, 0, { 4, 0, 0 } }, NotFound },    // ...
                  CreateTileDataAction{ { 4, 0, { 4, 0, 0 } } },           //
                  RetainTileDataAction{ { 4, 0, { 4, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 3, 0, { 3, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    // ...
              }),
              log);

    // Mark next level has having tried optional.
    log.clear();
    source.dataTiles[{ 4, 0, { 4, 0, 0 } }]->triedOptional = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 6);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 6, 0, { 6, 0, 0 } }, Found },       // ideal tile, not ready
                  RetainTileDataAction{ { 6, 0, { 6, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 7, 0, { 7, 0, 0 } }, NotFound },    // children
                  GetTileDataAction{ { 7, 0, { 7, 0, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 5, 0, { 5, 0, 0 } }, Found },       // ascent
                  RetainTileDataAction{ { 5, 0, { 5, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 4, 0, { 4, 0, 0 } }, Found },       // ...
                  RetainTileDataAction{ { 4, 0, { 4, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 3, 0, { 3, 0, 0 } }, NotFound },    // ...
                  CreateTileDataAction{ { 3, 0, { 3, 0, 0 } } },           //
                  RetainTileDataAction{ { 3, 0, { 3, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    // ...
              }),
              log);

    // Mark next level has having tried optional.
    log.clear();
    source.dataTiles[{ 3, 0, { 3, 0, 0 } }]->triedOptional = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 6);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 6, 0, { 6, 0, 0 } }, Found },       // ideal tile, not ready
                  RetainTileDataAction{ { 6, 0, { 6, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 7, 0, { 7, 0, 0 } }, NotFound },    // children
                  GetTileDataAction{ { 7, 0, { 7, 0, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 5, 0, { 5, 0, 0 } }, Found },       // ascent
                  RetainTileDataAction{ { 5, 0, { 5, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 4, 0, { 4, 0, 0 } }, Found },       // ...
                  RetainTileDataAction{ { 4, 0, { 4, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 3, 0, { 3, 0, 0 } }, Found },       // ...
                  RetainTileDataAction{ { 3, 0, { 3, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound },    // ...
                  CreateTileDataAction{ { 2, 0, { 2, 0, 0 } } },           //
                  RetainTileDataAction{ { 2, 0, { 2, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound },    // ...
              }),
              log);

    // Mark as found
    log.clear();
    auto tile_3_3_0_0 = source.dataTiles[{ 3, 0, { 3, 0, 0 } }].get();
    tile_3_3_0_0->renderable = true;
    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 6);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 6, 0, { 6, 0, 0 } }, Found },       // ideal tile, not ready
                  RetainTileDataAction{ { 6, 0, { 6, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 7, 0, { 7, 0, 0 } }, NotFound },    // children
                  GetTileDataAction{ { 7, 0, { 7, 0, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 0 } }, NotFound },    // ...
                  GetTileDataAction{ { 7, 0, { 7, 1, 1 } }, NotFound },    // ...
                  GetTileDataAction{ { 5, 0, { 5, 0, 0 } }, Found },       // ascent
                  RetainTileDataAction{ { 5, 0, { 5, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 4, 0, { 4, 0, 0 } }, Found },       // ...
                  RetainTileDataAction{ { 4, 0, { 4, 0, 0 } }, TileNecessity::Optional }, //
                  GetTileDataAction{ { 3, 0, { 3, 0, 0 } }, Found },       // ...
                  RetainTileDataAction{ { 3, 0, { 3, 0, 0 } }, TileNecessity::Optional }, //
                  RenderTileAction{ { 3, 0, 0 }, *tile_3_3_0_0 },       //
              }),
              log);
}

TEST(UpdateRenderables, LoadRequiredIfIdealTileCantBeFound) {
    ActionLog log;
    MockSource source;
    auto getTileData = getTileDataFn(log, source.dataTiles);
    auto createTileData = createTileDataFn(log, source.dataTiles);
    auto retainTileData = retainTileDataFn(log);
    auto renderTile = renderTileFn(log);

    source.zoomRange.max = 6;
    source.idealTiles.emplace(UnwrappedTileID{ 6, 0, 0 });

    auto tile_6_6_0_0 = source.createTileData(OverscaledTileID{ 6, 0, { 6, 0, 0 } });
    tile_6_6_0_0->triedOptional = true;
    tile_6_6_0_0->loaded = true;

    algorithm::updateRenderables(getTileData, createTileData, retainTileData, renderTile,
                                 source.idealTiles, source.zoomRange, 6);
    EXPECT_EQ(ActionLog({
                  GetTileDataAction{ { 6, 0, { 6, 0, 0 } }, Found }, // ideal tile, not found
                  RetainTileDataAction{ { 6, 0, { 6, 0, 0 } }, TileNecessity::Required }, //
                  GetTileDataAction{ { 7, 0, { 6, 0, 0 } }, NotFound }, // overzoomed child
                  GetTileDataAction{ { 5, 0, { 5, 0, 0 } }, NotFound }, // ascent
                  CreateTileDataAction{ { 5, 0, { 5, 0, 0 } } },
                  RetainTileDataAction{ { 5, 0, { 5, 0, 0 } }, TileNecessity::Required },
                  GetTileDataAction{ { 4, 0, { 4, 0, 0 } }, NotFound }, // ...
                  GetTileDataAction{ { 3, 0, { 3, 0, 0 } }, NotFound }, // ...
                  GetTileDataAction{ { 2, 0, { 2, 0, 0 } }, NotFound }, // ...
                  GetTileDataAction{ { 1, 0, { 1, 0, 0 } }, NotFound }, // ...
                  GetTileDataAction{ { 0, 0, { 0, 0, 0 } }, NotFound }, // ...
              }),
              log);
}
