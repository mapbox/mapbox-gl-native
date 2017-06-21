#include <benchmark/benchmark.h>

#include <mbgl/tile/vector_tile_data.hpp>
#include <mbgl/util/io.hpp>

using namespace mbgl;

static void Parse_VectorTile(benchmark::State& state) {
    auto data = std::make_shared<std::string>(util::read_file("test/fixtures/api/assets/streets/10-163-395.vector.pbf"));

    while (state.KeepRunning()) {
        std::size_t length = 0;
        VectorTileData tile(data);
        for (const auto& name : tile.layerNames()) {
            if (auto layer = tile.getLayer(name)) {
                const std::size_t count = layer->featureCount();
                for (std::size_t i = 0; i < count; i++) {
                    if (auto feature = layer->getFeature(i)) {
                        length += feature->getGeometries().size();
                        length += feature->getProperties().size();
                    }
                }
            }
        }
    }
}

BENCHMARK(Parse_VectorTile);
