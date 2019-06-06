#include <benchmark/benchmark.h>

#include <mbgl/algorithm/update_tile_masks.hpp>

using namespace mbgl;

class FakeTile {
public:
    FakeTile(TileMask mask_)
        : mask(std::move(mask_)) {
    }
    void setMask(TileMask mask_) {
        mask = std::move(mask_);
    }

    const bool usedByRenderedLayers = true;
    TileMask mask;
};

static void TileMaskGeneration(benchmark::State& state) {
    std::map<UnwrappedTileID, FakeTile> renderables = {
        { UnwrappedTileID{ 12, 1028, 1456 }, TileMask{} },
        { UnwrappedTileID{ 13, 2056, 2912 }, TileMask{} },
        { UnwrappedTileID{ 13, 2056, 2913 }, TileMask{} },
        { UnwrappedTileID{ 14, 4112, 5824 }, TileMask{} },
        { UnwrappedTileID{ 14, 4112, 5827 }, TileMask{} },
        { UnwrappedTileID{ 14, 4114, 5824 }, TileMask{} },
        { UnwrappedTileID{ 14, 4114, 5825 }, TileMask{} },
    };

    while (state.KeepRunning()) {
        algorithm::updateTileMasks(renderables);
    }
}

BENCHMARK(TileMaskGeneration);
