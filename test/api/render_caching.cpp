#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/offscreen_view.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/renderer/renderer.hpp>
// #include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/test/stub_renderer_frontend.hpp>

#include <future>
#include <memory>


TEST(API, RenderExpiredRasterTile) {
    using namespace mbgl;

    util::RunLoop loop;

    HeadlessBackend backend;
    BackendScope scope { backend };
    OffscreenView view { backend.getContext(), { 256, 512 } };
    float pixelRatio { 1 };
    DefaultFileSource db(":memory:");
    ThreadPool threadPool(4);
    StubRendererFrontend rendererFrontend { std::make_unique<Renderer>(backend, pixelRatio, fileSource, threadPool), view };

    // Log::setObserver(std::make_unique<FixtureLogObserver>());

    Map map { rendererFrontend, MapObserver::nullObserver(), view.getSize(), pixelRatio, fileSource,
        threadPool, MapMode::Still };

    map.getStyle().loadJSON(R"STYLE({
      "version": 8,
      "sources": {
        "raster": {
          "type": "raster",
          "tiles": ["https://tiles/{z}-{x}-{y}.png"]
        }
      },
      "layers": [{
        "id": "raster",
        "type": "raster",
        "source": "raster"
      }]
    })STYLE");
    }
    map.setLatLngZoom({ 37.8, -122.5 }, 10);

    test::checkImage("test/fixtures/api/cached_raster", test::render(map, view), 0.0006, 0.1);

    // auto observer = Log::removeObserver();
    // auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    // EXPECT_EQ(1u, flo->count(FixtureLog::Message(
    //                  EventSeverity::Error, Event::Style, -1,
    //                  std::string("Failed to load tile 0/0/0=>0 for source mapbox: " + message))));
    // auto unchecked = flo->unchecked();
    // EXPECT_TRUE(unchecked.empty()) << unchecked;
}
