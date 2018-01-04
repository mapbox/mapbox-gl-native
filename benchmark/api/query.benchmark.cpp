#include <benchmark/benchmark.h>

#include <mbgl/map/map.hpp>
#include <mbgl/gl/headless_frontend.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>

using namespace mbgl;

namespace {

class QueryBenchmark {
public:
    QueryBenchmark() {
        NetworkStatus::Set(NetworkStatus::Status::Offline);
        fileSource.setAccessToken("foobar");

        map.getStyle().loadJSON(util::read_file("benchmark/fixtures/api/style.json"));
        map.setLatLngZoom({ 40.726989, -73.992857 }, 15); // Manhattan
        map.getStyle().addImage(std::make_unique<style::Image>("test-icon",
            decodeImage(util::read_file("benchmark/fixtures/api/default_marker.png")), 1.0));

        frontend.render(map);
    }

    util::RunLoop loop;
    DefaultFileSource fileSource{ "benchmark/fixtures/api/cache.db", "." };
    ThreadPool threadPool{ 4 };
    HeadlessFrontend frontend { { 1000, 1000 }, 1, fileSource, threadPool };
    Map map { frontend, MapObserver::nullObserver(), frontend.getSize(), 1, fileSource, threadPool, MapMode::Static};
    ScreenBox box{{ 0, 0 }, { 1000, 1000 }};
};

} // end namespace

static void API_queryRenderedFeaturesAll(::benchmark::State& state) {
    QueryBenchmark bench;

    while (state.KeepRunning()) {
        bench.frontend.getRenderer()->queryRenderedFeatures(bench.box, {});
    }
}

static void API_queryRenderedFeaturesLayerFromLowDensity(::benchmark::State& state) {
    QueryBenchmark bench;

    while (state.KeepRunning()) {
        bench.frontend.getRenderer()->queryRenderedFeatures(bench.box, {{{ "testlayer" }}, {}});
    }
}

static void API_queryRenderedFeaturesLayerFromHighDensity(::benchmark::State& state) {
    QueryBenchmark bench;

    while (state.KeepRunning()) {
        bench.frontend.getRenderer()->queryRenderedFeatures(bench.box, {{{"road-street" }}, {}});
    }
}

BENCHMARK(API_queryRenderedFeaturesAll);
BENCHMARK(API_queryRenderedFeaturesLayerFromLowDensity);
BENCHMARK(API_queryRenderedFeaturesLayerFromHighDensity);
