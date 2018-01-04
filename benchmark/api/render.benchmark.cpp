#include <benchmark/benchmark.h>

#include <mbgl/map/map.hpp>
#include <mbgl/map/map_observer.hpp>
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

class RenderBenchmark {
public:
    RenderBenchmark() {
        NetworkStatus::Set(NetworkStatus::Status::Offline);
        fileSource.setAccessToken("foobar");
    }

    util::RunLoop loop;
    DefaultFileSource fileSource { "benchmark/fixtures/api/cache.db", "." };
    ThreadPool threadPool { 4 };
};
    
static void prepare(Map& map, optional<std::string> json = {}) {
    map.getStyle().loadJSON(json ? *json : util::read_file("benchmark/fixtures/api/style.json"));
    map.setLatLngZoom({ 40.726989, -73.992857 }, 15); // Manhattan
    map.getStyle().addImage(std::make_unique<style::Image>("test-icon",
                                                           decodeImage(util::read_file("benchmark/fixtures/api/default_marker.png")), 1.0));
}
 
} // end namespace

static void API_renderStill_reuse_map(::benchmark::State& state) {
    RenderBenchmark bench;
    HeadlessFrontend frontend { { 1000, 1000 }, 1, bench.fileSource, bench.threadPool };
    Map map { frontend, MapObserver::nullObserver(), frontend.getSize(), 1, bench.fileSource, bench.threadPool, MapMode::Static};
    prepare(map);

    while (state.KeepRunning()) {
        frontend.render(map);
    }
}

static void API_renderStill_reuse_map_switch_styles(::benchmark::State& state) {
    RenderBenchmark bench;
    HeadlessFrontend frontend { { 1000, 1000 }, 1, bench.fileSource, bench.threadPool };
    Map map { frontend, MapObserver::nullObserver(), frontend.getSize(), 1, bench.fileSource, bench.threadPool, MapMode::Static};
    
    while (state.KeepRunning()) {
        prepare(map, { "{}" });
        frontend.render(map);
        prepare(map);
        frontend.render(map);
    }
}

static void API_renderStill_recreate_map(::benchmark::State& state) {
    RenderBenchmark bench;
    
    while (state.KeepRunning()) {
        HeadlessFrontend frontend { { 1000, 1000 }, 1, bench.fileSource, bench.threadPool };
        Map map { frontend, MapObserver::nullObserver(), frontend.getSize(), 1, bench.fileSource, bench.threadPool, MapMode::Static};
        prepare(map);
        frontend.render(map);
    }
}

BENCHMARK(API_renderStill_reuse_map);
BENCHMARK(API_renderStill_reuse_map_switch_styles);
BENCHMARK(API_renderStill_recreate_map);
