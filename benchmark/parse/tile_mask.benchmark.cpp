#include <benchmark/benchmark.h>

#include <mbgl/algorithm/update_tile_masks.hpp>

using namespace mbgl;

struct MaskedRenderable {
    UnwrappedTileID id;
    TileMask mask;
    bool used = true;
};

static void TileMaskGeneration(benchmark::State& state) {
    std::vector<MaskedRenderable> renderables = {
        MaskedRenderable{ UnwrappedTileID{ 12, 1028, 1456 }, {} },
        MaskedRenderable{ UnwrappedTileID{ 13, 2056, 2912 }, {} },
        MaskedRenderable{ UnwrappedTileID{ 13, 2056, 2913 }, {} },
        MaskedRenderable{ UnwrappedTileID{ 14, 4112, 5824 }, {} },
        MaskedRenderable{ UnwrappedTileID{ 14, 4112, 5827 }, {} },
        MaskedRenderable{ UnwrappedTileID{ 14, 4114, 5824 }, {} },
        MaskedRenderable{ UnwrappedTileID{ 14, 4114, 5825 }, {} },
    };

    while (state.KeepRunning()) {
        algorithm::updateTileMasks<MaskedRenderable>({ renderables.begin(), renderables.end() });
    }
}

BENCHMARK(TileMaskGeneration);
