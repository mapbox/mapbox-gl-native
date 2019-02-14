#include <benchmark/benchmark.h>

#include <mbgl/map/map.hpp>
#include <mbgl/map/map_observer.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/gl/headless_frontend.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>

using namespace mbgl;

namespace {

static std::string cachePath { "benchmark/fixtures/api/cache.db" };
constexpr double pixelRatio { 1.0 };
constexpr Size size { 1000, 1000 };

class RenderBenchmark {
public:
    RenderBenchmark() {
        NetworkStatus::Set(NetworkStatus::Status::Offline);
    }

    util::RunLoop loop;
};

static void prepare(Map& map, optional<std::string> json = {}) {
    map.getStyle().loadJSON(json ? *json : util::read_file("benchmark/fixtures/api/style.json"));
    map.jumpTo(CameraOptions().withCenter(LatLng { 40.726989, -73.992857 }).withZoom(15.0)); // Manhattan

    auto image = decodeImage(util::read_file("benchmark/fixtures/api/default_marker.png"));
    map.getStyle().addImage(std::make_unique<style::Image>("test-icon", std::move(image), 1.0));
}

} // end namespace

static void API_renderStill_reuse_map(::benchmark::State& state) {
    RenderBenchmark bench;
    HeadlessFrontend frontend { size, pixelRatio };
    Map map { frontend, MapObserver::nullObserver(),
              MapOptions().withMapMode(MapMode::Static).withSize(size).withPixelRatio(pixelRatio),
              ResourceOptions().withCachePath(cachePath).withAccessToken("foobar") };
    prepare(map);

    while (state.KeepRunning()) {
        frontend.render(map);
    }
}

static void API_renderStill_reuse_map_formatted_labels(::benchmark::State& state) {
    RenderBenchmark bench;
    HeadlessFrontend frontend { size, pixelRatio };
    Map map { frontend, MapObserver::nullObserver(),
              MapOptions().withMapMode(MapMode::Static).withSize(size).withPixelRatio(pixelRatio),
              ResourceOptions().withCachePath(cachePath).withAccessToken("foobar") };
    prepare(map, util::read_file("benchmark/fixtures/api/style_formatted_labels.json"));

    while (state.KeepRunning()) {
        frontend.render(map);
    }
}

static void API_renderStill_reuse_map_switch_styles(::benchmark::State& state) {
    RenderBenchmark bench;
    HeadlessFrontend frontend { size, pixelRatio };
    Map map { frontend, MapObserver::nullObserver(),
              MapOptions().withMapMode(MapMode::Static).withSize(size).withPixelRatio(pixelRatio),
              ResourceOptions().withCachePath(cachePath).withAccessToken("foobar") };

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
        HeadlessFrontend frontend { size, pixelRatio };
        Map map { frontend, MapObserver::nullObserver(),
                  MapOptions().withMapMode(MapMode::Static).withSize(size).withPixelRatio(pixelRatio),
                  ResourceOptions().withCachePath(cachePath).withAccessToken("foobar") };
        prepare(map);
        frontend.render(map);
    }
}

BENCHMARK(API_renderStill_reuse_map);
BENCHMARK(API_renderStill_reuse_map_formatted_labels);
BENCHMARK(API_renderStill_reuse_map_switch_styles);
BENCHMARK(API_renderStill_recreate_map);
