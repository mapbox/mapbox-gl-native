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

class FrameObserver : public mbgl::MapObserver {
public:
    void onDidFinishRenderingFrame(RenderMode mode) {
        if (mode == RenderMode::Full) {
//            fprintf(stderr, "onDidFinishRenderingFrame: full\n");
//            mbgl::util::RunLoop::Get()->stop();
        } else {
//            fprintf(stderr, "onDidFinishRenderingFrame: partial\n");
        }
    }
    void onDidFinishRenderingMap(RenderMode mode) {
//        fprintf(stderr, "onDidFinishRenderingMap\n");
        if (mode == RenderMode::Full) {
//            fprintf(stderr, "onDidFinishRenderingMap: full\n");
            mbgl::util::RunLoop::Get()->stop();
        } else {
//            fprintf(stderr, "onDidFinishRenderingMap: partial\n");
        }
    }
};


TEST(API, RenderExpiredRasterTile) {
    using namespace mbgl;

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    util::RunLoop loop;

    HeadlessBackend backend;
    BackendScope scope { backend };
    OffscreenView view { backend.getContext(), { 512, 256 } };
    float pixelRatio { 1 };
    DefaultFileSource fileSource(":memory:", "test/fixtures/api/assets");
    fileSource.setOnlineStatus(false);
    ThreadPool threadPool(4);
    StubRendererFrontend rendererFrontend { std::make_unique<Renderer>(backend, pixelRatio, fileSource, threadPool), view };

    Response response;
    response.expires = util::parseTimestamp("Tue, 04 Nov 2014 02:13:24 GMT");

    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/assets/raster/6-10-24.png"));
    fileSource.put(Resource::tile("https://tiles/{z}-{x}-{y}.png", 1, 10, 24, 6, Tileset::Scheme::XYZ), response);
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/assets/raster/10-163-395.png"));
    fileSource.put(Resource::tile("https://tiles/{z}-{x}-{y}.png", 1, 163, 395, 10, Tileset::Scheme::XYZ), response);


    FrameObserver observer;
    Map map { rendererFrontend, observer, view.getSize(), pixelRatio, fileSource,
        threadPool, MapMode::Continuous };

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

    map.setLatLngZoom({ 37.8, -122.4 }, 10);

    loop.run();

    PremultipliedImage result = view.readStillImage();
    test::checkImage("test/fixtures/api/cached_raster", result, 0.0006, 0.1);

//    auto flo = dynamic_cast<FixtureLogObserver*>(Log::removeObserver().get());
//    EXPECT_EQ(1u, flo->count(FixtureLog::Message(EventSeverity::Error, Event::Style, -1,
//        "Failed to load tile 10/163/395=>10 for source raster: Online connectivity is disabled.")));
//    EXPECT_EQ(1u, flo->count(FixtureLog::Message(EventSeverity::Error, Event::Style, -1,
//        "Failed to load tile 10/164/395=>10 for source raster: Online connectivity is disabled.")));
//    EXPECT_EQ(1u, flo->count(FixtureLog::Message(EventSeverity::Error, Event::Style, -1,
//        "Failed to load tile 9/81/197=>9 for source raster: Online connectivity is disabled.")));
//    EXPECT_EQ(1u, flo->count(FixtureLog::Message(EventSeverity::Error, Event::Style, -1,
//        "Failed to load tile 9/82/197=>9 for source raster: Online connectivity is disabled.")));
//    EXPECT_EQ(1u, flo->count(FixtureLog::Message(EventSeverity::Error, Event::Style, -1,
//        "Failed to load tile 8/40/98=>8 for source raster: Online connectivity is disabled.")));
//    EXPECT_EQ(1u, flo->count(FixtureLog::Message(EventSeverity::Error, Event::Style, -1,
//        "Failed to load tile 8/41/98=>8 for source raster: Online connectivity is disabled.")));
//    EXPECT_EQ(1u, flo->count(FixtureLog::Message(EventSeverity::Error, Event::Style, -1,
//        "Failed to load tile 7/20/49=>7 for source raster: Online connectivity is disabled.")));
//    EXPECT_EQ(1u, flo->count(FixtureLog::Message(EventSeverity::Error, Event::Style, -1,
//        "Failed to load tile 6/10/24=>6 for source raster: Online connectivity is disabled.")));
//    EXPECT_EQ(1u, flo->count(FixtureLog::Message(EventSeverity::Error, Event::Style, -1,
//        "Failed to load tile 5/5/12=>5 for source raster: Online connectivity is disabled.")));
//    EXPECT_EQ(1u, flo->count(FixtureLog::Message(EventSeverity::Error, Event::Style, -1,
//        "Failed to load tile 4/2/6=>4 for source raster: Online connectivity is disabled.")));
//    EXPECT_EQ(1u, flo->count(FixtureLog::Message(EventSeverity::Error, Event::Style, -1,
//        "Failed to load tile 3/1/3=>3 for source raster: Online connectivity is disabled.")));
//    EXPECT_EQ(1u, flo->count(FixtureLog::Message(EventSeverity::Error, Event::Style, -1,
//        "Failed to load tile 2/0/1=>2 for source raster: Online connectivity is disabled.")));
//    EXPECT_EQ(1u, flo->count(FixtureLog::Message(EventSeverity::Error, Event::Style, -1,
//        "Failed to load tile 1/0/0=>1 for source raster: Online connectivity is disabled.")));
//    EXPECT_EQ(1u, flo->count(FixtureLog::Message(EventSeverity::Error, Event::Style, -1,
//        "Failed to load tile 0/0/0=>0 for source raster: Online connectivity is disabled.")));
//    auto unchecked = flo->unchecked();
//    EXPECT_TRUE(unchecked.empty()) << unchecked;
}
