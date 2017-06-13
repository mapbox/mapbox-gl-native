#include <benchmark/benchmark.h>

#include <mbgl/benchmark/util.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/map_observer.hpp>
#include <mbgl/map/backend_scope.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/offscreen_view.hpp>
#include <mbgl/util/default_thread_pool.hpp>
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
    HeadlessBackend backend;
    BackendScope scope { backend };
    OffscreenView view { backend.getContext(), { 1000, 1000 } };
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
    Map map { bench.backend, MapObserver::nullObserver(), bench.view.getSize(), 1, bench.fileSource, bench.threadPool, MapMode::Still };
    prepare(map);

    while (state.KeepRunning()) {
        mbgl::benchmark::render(map, bench.view);
    }
}

static void API_renderStill_reuse_map_switch_styles(::benchmark::State& state) {
    RenderBenchmark bench;
    Map map { bench.backend, MapObserver::nullObserver(), bench.view.getSize(), 1, bench.fileSource, bench.threadPool, MapMode::Still };
    
    while (state.KeepRunning()) {
        prepare(map, { "{}" });
        mbgl::benchmark::render(map, bench.view);
        prepare(map);
        mbgl::benchmark::render(map, bench.view);
    }
}

static void API_renderStill_recreate_map(::benchmark::State& state) {
    RenderBenchmark bench;
    
    while (state.KeepRunning()) {
        Map map { bench.backend, MapObserver::nullObserver(), bench.view.getSize(), 1, bench.fileSource, bench.threadPool, MapMode::Still };
        prepare(map);
        mbgl::benchmark::render(map, bench.view);
    }
}

BENCHMARK(API_renderStill_reuse_map);
BENCHMARK(API_renderStill_reuse_map_switch_styles);
BENCHMARK(API_renderStill_recreate_map);
