#include <benchmark/benchmark.h>

#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/sqlite3.hpp>
#include <mbgl/util/string.hpp>

class OfflineDatabase : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State&) override {
        using namespace mbgl;
        using namespace std::chrono_literals;

        const unsigned tileCount = 10000;
        db.setOfflineMapboxTileCountLimit(tileCount * 2);

        Response response;
        response.noContent = true;
        response.mustRevalidate = false;
        response.expires = util::now() + 1h;

        for (unsigned i = 0; i < tileCount; ++i) {
            const Resource ambient = Resource::tile("mapbox://tile_ambient" + util::toString(i), 1, 0, 0, 0, Tileset::Scheme::XYZ);
            db.put(ambient, response);
        }

        OfflineTilePyramidRegionDefinition definition{ "mapbox://style", LatLngBounds::hull({1, 2}, {3, 4}), 5, 6, 2.0, true };
        OfflineRegionMetadata metadata{{ 1, 2, 3 }};

        auto region = db.createRegion(definition, metadata);
        regionID = region->getID();

        for (unsigned i = 0; i < tileCount; ++i) {
            const Resource offline = Resource::tile("mapbox://tile_offline_region" + util::toString(i), 1.0, 0, 0, 0, Tileset::Scheme::XYZ);
            db.putRegionResource(regionID, offline, response);
        }
    }

    mbgl::OfflineDatabase db{":memory:"};
    int64_t regionID;
};

BENCHMARK_F(OfflineDatabase, InvalidateRegion)(benchmark::State& state) {
    for (auto _ : state) {
        db.invalidateRegion(regionID);
    }
}

BENCHMARK_F(OfflineDatabase, InvalidateTileCache)(benchmark::State& state) {
    for (auto _ : state) {
        db.invalidateTileCache();
    }
}
