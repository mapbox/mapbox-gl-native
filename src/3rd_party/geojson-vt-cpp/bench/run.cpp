#include <mapbox/geojson.hpp>
#include <mapbox/geojsonvt.hpp>

#include "util.hpp"

#include <fstream>
#include <iostream>

int main() {
    Timer timer;

    const std::string json = loadFile("data/countries.geojson");
    timer("read file");

    const auto features = mapbox::geojson::parse(json).get<mapbox::geojson::feature_collection>();
    timer("parse into geometry");

    mapbox::geojsonvt::Options options;
    options.indexMaxZoom = 7;
    options.indexMaxPoints = 200;

    mapbox::geojsonvt::GeoJSONVT index{ features, options };

    for (uint32_t i = 0; i < 100; i++) {
        mapbox::geojsonvt::GeoJSONVT index{ features, options };
    }
    timer("generate tile index 100 times");

    printf("tiles generated: %i {\n", static_cast<int>(index.total));
    for (const auto& pair : index.stats) {
        printf("    z%i: %i\n", pair.first, pair.second);
    }
    printf("}\n");

    const unsigned max_z = 11;
    std::size_t count = 0;
    for (unsigned z = 0; z < max_z; ++z) {
        unsigned num_tiles = std::pow(2, z);
        for (unsigned x = 0; x < num_tiles; ++x) {
            for (unsigned y = 0; y < num_tiles; ++y) {
                auto const& tile = index.getTile(z, x, y);
                count += tile.features.size();
            }
        }
    }
    timer("getTile, found " + std::to_string(count) + " features");
}
