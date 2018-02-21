#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/fake_file_source.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/headless_frontend.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/util/color.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace std::literals::string_literals;

class StubMapObserver : public MapObserver {
public:
    void onWillStartLoadingMap() final {
        if (onWillStartLoadingMapCallback) {
            onWillStartLoadingMapCallback();
        }
    }
    
    void onDidFinishLoadingMap() final {
        if (onDidFinishLoadingMapCallback) {
            onDidFinishLoadingMapCallback();
        }
    }
    
    void onDidFailLoadingMap(std::exception_ptr) final {
        if (didFailLoadingMapCallback) {
            didFailLoadingMapCallback();
        }
    }
    
    void onDidFinishLoadingStyle() final {
        if (didFinishLoadingStyleCallback) {
            didFinishLoadingStyleCallback();
        }
    }

    void onDidFinishRenderingFrame(RenderMode mode) final {
        if (didFinishRenderingFrame) {
            didFinishRenderingFrame(mode);
        }
    }

    std::function<void()> onWillStartLoadingMapCallback;
    std::function<void()> onDidFinishLoadingMapCallback;
    std::function<void()> didFailLoadingMapCallback;
    std::function<void()> didFinishLoadingStyleCallback;
    std::function<void(RenderMode)> didFinishRenderingFrame;
};

template <class FileSource = StubFileSource>
class MapTest {
public:
    util::RunLoop runLoop;
    FileSource fileSource;
    ThreadPool threadPool { 4 };
    StubMapObserver observer;
    HeadlessFrontend frontend;
    Map map;

    MapTest(float pixelRatio = 1, MapMode mode = MapMode::Still)
        : frontend(pixelRatio, fileSource, threadPool)
        , map(frontend, observer, frontend.getSize(), pixelRatio, fileSource, threadPool, mode) {
    }

    template <typename T = FileSource>
    MapTest(const std::string& cachePath, const std::string& assetRoot,
            float pixelRatio = 1, MapMode mode = MapMode::Still,
            typename std::enable_if<std::is_same<T, DefaultFileSource>::value>::type* = 0)
            : fileSource { cachePath, assetRoot }
            , frontend(pixelRatio, fileSource, threadPool)
            , map(frontend, observer, frontend.getSize(), pixelRatio, fileSource, threadPool, mode) {
    }
};

TEST(Map, LatLngBehavior) {
    MapTest<> test;

    test.map.setLatLngZoom({ 1, 1 }, 0);
    auto latLng1 = test.map.getLatLng();

    test.map.setLatLng({ 1, 1 });
    auto latLng2 = test.map.getLatLng();

    ASSERT_DOUBLE_EQ(latLng1.latitude(), latLng2.latitude());
    ASSERT_DOUBLE_EQ(latLng1.longitude(), latLng2.longitude());
}

TEST(Map, LatLngBoundsToCamera) {
    MapTest<> test;

    test.map.setLatLngZoom({ 40.712730, -74.005953 }, 16.0);

    LatLngBounds bounds = LatLngBounds::hull({15.68169,73.499857}, {53.560711, 134.77281});

    CameraOptions virtualCamera = test.map.cameraForLatLngBounds(bounds, {});
    ASSERT_TRUE(bounds.contains(*virtualCamera.center));
    EXPECT_NEAR(*virtualCamera.zoom, 1.55467, 1e-5);
}

TEST(Map, LatLngBoundsToCameraWithAngle) {
    MapTest<> test;

    test.map.setLatLngZoom({ 40.712730, -74.005953 }, 16.0);

    LatLngBounds bounds = LatLngBounds::hull({15.68169,73.499857}, {53.560711, 134.77281});

    CameraOptions virtualCamera = test.map.cameraForLatLngBounds(bounds, {}, 35);
    ASSERT_TRUE(bounds.contains(*virtualCamera.center));
    EXPECT_NEAR(*virtualCamera.zoom, 1.21385, 1e-5);
    EXPECT_DOUBLE_EQ(virtualCamera.angle.value_or(0), -35 * util::DEG2RAD);
}

TEST(Map, LatLngsToCamera) {
    MapTest<> test;

    std::vector<LatLng> latLngs{{ 40.712730, 74.005953 }, {15.68169,73.499857}, {30.82678, 83.4082}};

    CameraOptions virtualCamera = test.map.cameraForLatLngs(latLngs, {}, 23);
    EXPECT_DOUBLE_EQ(virtualCamera.angle.value_or(0), -23 * util::DEG2RAD);
    EXPECT_NEAR(virtualCamera.zoom.value_or(0), 2.75434, 1e-5);
    EXPECT_NEAR(virtualCamera.center->latitude(), 28.49288, 1e-5);
    EXPECT_NEAR(virtualCamera.center->longitude(), 74.97437, 1e-5);
}

TEST(Map, CameraToLatLngBounds) {
    MapTest<> test;

    test.map.setLatLngZoom({ 45, 90 }, 16);

    LatLngBounds bounds = LatLngBounds::hull(
            test.map.latLngForPixel({}),
            test.map.latLngForPixel({ double(test.map.getSize().width), double(test.map.getSize().height) }));

    CameraOptions camera = test.map.getCameraOptions({});

    ASSERT_EQ(bounds, test.map.latLngBoundsForCamera(camera));

    // Map::cameraForLatLngBounds only sets zoom and center.
    CameraOptions virtualCamera = test.map.cameraForLatLngBounds(bounds, {});
    ASSERT_NEAR(*camera.zoom, *virtualCamera.zoom, 1e-7);
    ASSERT_NEAR(camera.center->latitude(), virtualCamera.center->latitude(), 1e-7);
    ASSERT_NEAR(camera.center->longitude(), virtualCamera.center->longitude(), 1e-7);
}

TEST(Map, Offline) {
    MapTest<DefaultFileSource> test {":memory:", "."};

    auto expiredItem = [] (const std::string& path) {
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/map/offline/"s + path));
        response.expires = Timestamp{ Seconds(0) };
        return response;
    };

    const std::string prefix = "http://127.0.0.1:3000/";
    test.fileSource.put(Resource::style(prefix + "style.json"), expiredItem("style.json"));
    test.fileSource.put(Resource::source(prefix + "streets.json"), expiredItem("streets.json"));
    test.fileSource.put(Resource::spriteJSON(prefix + "sprite", 1.0), expiredItem("sprite.json"));
    test.fileSource.put(Resource::spriteImage(prefix + "sprite", 1.0), expiredItem("sprite.png"));
    test.fileSource.put(Resource::tile(prefix + "{z}-{x}-{y}.vector.pbf", 1.0, 0, 0, 0, Tileset::Scheme::XYZ), expiredItem("0-0-0.vector.pbf"));
    test.fileSource.put(Resource::glyphs(prefix + "{fontstack}/{range}.pbf", {{"Helvetica"}}, {0, 255}), expiredItem("glyph.pbf"));
    NetworkStatus::Set(NetworkStatus::Status::Offline);

    test.map.getStyle().loadURL(prefix + "style.json");

    test::checkImage("test/fixtures/map/offline",
                     test.frontend.render(test.map),
                     0.0015,
                     0.1);

    NetworkStatus::Set(NetworkStatus::Status::Online);
}

TEST(Map, SetStyleDefaultCamera) {
    MapTest<> test;
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    EXPECT_DOUBLE_EQ(test.map.getZoom(), 0.0);
    EXPECT_DOUBLE_EQ(test.map.getPitch(), 0.0);
    EXPECT_DOUBLE_EQ(test.map.getBearing(), 0.0);
    EXPECT_EQ(test.map.getLatLng(), LatLng {});

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty-zoomed.json"));
    EXPECT_DOUBLE_EQ(test.map.getZoom(), 0.0);

    test.map.jumpTo(test.map.getStyle().getDefaultCamera());
    EXPECT_DOUBLE_EQ(test.map.getZoom(), 0.5);
}

TEST(Map, SetStyleInvalidJSON) {
    Log::setObserver(std::make_unique<FixtureLogObserver>());

    bool fail = false;

    {
        MapTest<> test;
        test.observer.didFailLoadingMapCallback = [&]() {
            fail = true;
        };
        test.map.getStyle().loadJSON("invalid");
    }

    EXPECT_TRUE(fail);

    auto observer = Log::removeObserver();
    auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    EXPECT_EQ(1u, flo->count({ EventSeverity::Error, Event::ParseStyle, -1,
        "Failed to parse style: 0 - Invalid value." }));
    auto unchecked = flo->unchecked();
    EXPECT_TRUE(unchecked.empty()) << unchecked;
}

TEST(Map, SetStyleInvalidURL) {
    MapTest<> test;

    test.fileSource.styleResponse = [] (const Resource&) {
        Response response;
        response.error = std::make_unique<Response::Error>(
            Response::Error::Reason::Other,
            "Failed by the test case");
        return response;
    };

    test.observer.didFailLoadingMapCallback = [&]() {
        test.runLoop.stop();
    };

    test.map.getStyle().loadURL("mapbox://bar");

    test.runLoop.run();
}

TEST(Map, DoubleStyleLoad) {
    MapTest<> test;

    test.map.getStyle().loadJSON("");
    test.map.getStyle().loadJSON("");
}

TEST(Map, StyleFresh) {
    // The map should continue to revalidate fresh styles.

    MapTest<FakeFileSource> test;

    test.map.getStyle().loadURL("mapbox://styles/test");
    EXPECT_EQ(1u, test.fileSource.requests.size());

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/empty.json"));
    response.expires = Timestamp::max();

    test.fileSource.respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource.requests.size());
}

TEST(Map, StyleExpired) {
    // The map should allow expired styles to be revalidated until we get a fresh style.

    using namespace std::chrono_literals;

    MapTest<FakeFileSource> test;

    test.map.getStyle().loadURL("mapbox://styles/test");
    EXPECT_EQ(1u, test.fileSource.requests.size());

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/empty.json"));
    response.expires = util::now() - 1h;

    test.fileSource.respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource.requests.size());

    // Mutate layer. From now on, sending a response to the style won't overwrite it anymore, but
    // we should continue to wait for a fresh response.
    test.map.getStyle().addLayer(std::make_unique<style::BackgroundLayer>("bg"));
    EXPECT_EQ(1u, test.fileSource.requests.size());

    // Send another expired response, and confirm that we didn't overwrite the style, but continue
    // to wait for a fresh response.
    test.fileSource.respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource.requests.size());
    EXPECT_NE(nullptr, test.map.getStyle().getLayer("bg"));

    // Send a fresh response, and confirm that we didn't overwrite the style, but continue to wait
    // for a fresh response.
    response.expires = util::now() + 1h;
    test.fileSource.respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource.requests.size());
    EXPECT_NE(nullptr, test.map.getStyle().getLayer("bg"));
}

TEST(Map, StyleExpiredWithAnnotations) {
    // Adding an annotation should not prevent revalidation of an expired style.

    using namespace std::chrono_literals;

    MapTest<FakeFileSource> test;

    test.map.getStyle().loadURL("mapbox://styles/test");
    EXPECT_EQ(1u, test.fileSource.requests.size());

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/empty.json"));
    response.expires = util::now() - 1h;

    test.fileSource.respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource.requests.size());

    test.map.addAnnotation(LineAnnotation { LineString<double> {{ { 0, 0 }, { 10, 10 } }} });
    EXPECT_EQ(1u, test.fileSource.requests.size());

    test.fileSource.respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource.requests.size());
}

TEST(Map, StyleExpiredWithRender) {
    // Rendering should not prevent revalidation of an expired style.

    using namespace std::chrono_literals;

    MapTest<FakeFileSource> test;

    test.map.getStyle().loadURL("mapbox://styles/test");
    EXPECT_EQ(1u, test.fileSource.requests.size());

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/empty.json"));
    response.expires = util::now() - 1h;

    test.fileSource.respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource.requests.size());

    test.frontend.render(test.map);
    EXPECT_EQ(1u, test.fileSource.requests.size());

    test.fileSource.respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource.requests.size());
}

TEST(Map, StyleEarlyMutation) {
    // An early mutation should not prevent the initial style load.

    MapTest<FakeFileSource> test;

    test.map.getStyle().loadURL("mapbox://styles/test");
    test.map.getStyle().addLayer(std::make_unique<style::BackgroundLayer>("bg"));

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/water.json"));
    test.fileSource.respond(Resource::Style, response);

    EXPECT_EQ(1u, test.fileSource.requests.size());
    EXPECT_NE(nullptr, test.map.getStyle().getLayer("water"));
}

TEST(Map, MapLoadingSignal) {
    MapTest<> test;

    bool emitted = false;
    test.observer.onWillStartLoadingMapCallback = [&]() {
        emitted = true;
    };
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    EXPECT_TRUE(emitted);
}

TEST(Map, MapLoadedSignal) {
    MapTest<> test { 1, MapMode::Continuous };

    test.observer.onDidFinishLoadingMapCallback = [&]() {
        test.runLoop.stop();
    };

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.runLoop.run();
}

TEST(Map, StyleLoadedSignal) {
    MapTest<> test;

    // The map should emit a signal on style loaded
    bool emitted = false;
    test.observer.didFinishLoadingStyleCallback = [&]() {
        emitted = true;
    };
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    EXPECT_TRUE(emitted);

    // But not when the style couldn't be parsed
    emitted = false;
    test.map.getStyle().loadJSON("invalid");
    EXPECT_FALSE(emitted);
}

// Test for https://github.com/mapbox/mapbox-gl-native/issues/7902
TEST(Map, TEST_REQUIRES_SERVER(StyleNetworkErrorRetry)) {
    MapTest<OnlineFileSource> test;

    test.map.getStyle().loadURL("http://127.0.0.1:3000/style-fail-once-500");

    test.observer.didFinishLoadingStyleCallback = [&]() {
        test.runLoop.stop();
    };

    test.runLoop.run();
}

TEST(Map, TEST_REQUIRES_SERVER(StyleNotFound)) {
    MapTest<OnlineFileSource> test;

    test.map.getStyle().loadURL("http://127.0.0.1:3000/style-fail-once-404");

    using namespace std::chrono_literals;
    util::Timer timer;

    // Not found errors should not trigger a retry like other errors.
    test.observer.didFinishLoadingStyleCallback = [&]() {
        FAIL() << "Should not retry on not found!";
    };

    test.observer.didFailLoadingMapCallback = [&]() {
        timer.start(Milliseconds(1100), 0s, [&] {
            test.runLoop.stop();
        });
    };

    test.runLoop.run();

    // Should also not retry if the response has cache headers.
    test.map.getStyle().loadURL("http://127.0.0.1:3000/style-fail-once-404-cache");
    test.runLoop.run();
}

TEST(Map, AddLayer) {
    MapTest<> test;

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));

    auto layer = std::make_unique<BackgroundLayer>("background");
    layer->setBackgroundColor({ { 1, 0, 0, 1 } });
    test.map.getStyle().addLayer(std::move(layer));

    test::checkImage("test/fixtures/map/add_layer", test.frontend.render(test.map));
}

TEST(Map, WithoutVAOExtension) {
    MapTest<DefaultFileSource> test { ":memory:", "test/fixtures/api/assets" };

    BackendScope scope { *test.frontend.getBackend() };
    test.frontend.getBackend()->getContext().disableVAOExtension = true;

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/water.json"));

    test::checkImage("test/fixtures/map/no_vao", test.frontend.render(test.map), 0.002);
}

TEST(Map, RemoveLayer) {
    MapTest<> test;

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));

    auto layer = std::make_unique<BackgroundLayer>("background");
    layer->setBackgroundColor({{ 1, 0, 0, 1 }});
    test.map.getStyle().addLayer(std::move(layer));
    test.map.getStyle().removeLayer("background");

    test::checkImage("test/fixtures/map/remove_layer", test.frontend.render(test.map));
}

TEST(Map, DisabledSources) {
    MapTest<> test;

    // Always load the same image tile for raster layers.
    test.fileSource.response = [] (const Resource& res) -> optional<Response> {
        if (res.url == "asset://tile.png") {
            Response response;
            response.data = std::make_shared<std::string>(
                util::read_file("test/fixtures/map/disabled_layers/tile.png"));
            return {std::move(response)};
        }
        return {};
    };

    test.map.setZoom(1);

    // This stylesheet has two raster layers, one that starts at zoom 1, the other at zoom 0.
    // We first render a map at zoom level 1, which should show both layers (both are "visible" due
    // to an opacity of 0.5). Then, we are zooming back out to a zoom level of 0.5 and rerender.
    // The "raster1" layer should not be visible anymore since it has minzoom 1, while "raster2"
    // should still be there. Both layers have a distinct color through "raster-hue-rotate".
    test.map.getStyle().loadJSON(R"STYLE(
{
  "version": 8,
  "name": "Test",
  "sources": {
    "raster": {
      "type": "raster",
      "tiles": [ "asset://tile.png" ],
      "tileSize": 256
    }
  },
  "layers": [{
    "id": "background",
    "type": "background",
    "paint": {
      "background-color": "white"
    }
  }, {
    "id": "raster1",
    "type": "raster",
    "source": "raster",
    "minzoom": 0
  }, {
    "id": "raster2",
    "type": "raster",
    "source": "raster",
    "minzoom": 1,
    "paint": {
      "raster-hue-rotate": 180
    }
  }]
}
)STYLE");

    test::checkImage("test/fixtures/map/disabled_layers/first", test.frontend.render(test.map));
    test.map.setZoom(0.5);
    test::checkImage("test/fixtures/map/disabled_layers/second", test.frontend.render(test.map));
}

TEST(Map, DontLoadUnneededTiles) {
    MapTest<> test;

    test.map.getStyle().loadJSON(R"STYLE({
  "sources": {
    "a": { "type": "vector", "tiles": [ "a/{z}/{x}/{y}" ] }
  },
  "layers": [{
    "id": "a",
    "type": "fill",
    "source": "a",
    "source-layer": "a",
    "minzoom": 0.3,
    "maxzoom": 1.6
  }]
})STYLE");

    using Tiles = std::unordered_set<std::string>;
    Tiles tiles;

    test.fileSource.tileResponse = [&](const Resource& rsc) {
        tiles.emplace(rsc.url);
        Response res;
        res.noContent = true;
        return res;
    };

    std::unordered_map<double, Tiles> referenceTiles = {
        // Since the layer's minzoom is 0.3, we shouldn't load tiles before z0.3
        { 0.3, { "a/0/0/0" } },
        { 1.0, { "a/1/1/0", "a/1/0/1", "a/1/0/0", "a/1/1/1" } },
        // Since the layer's maxzoom is 1.6, we should never load z2 or z3 tiles.
    };

    // Loop through zoom levels from 0 to 3 and check that the correct tiles are loaded at every
    // step. The source is marked with maxzoom 1.0, which means that it won't be visible anymore
    // after z1.0, so we should under no circumstances attempt to load z2 tiles.
    for (unsigned zoom = 0; zoom <= 30; zoom++) { // times 10
        // Note: using z += 0.1 in the loop doesn't produce accurate floating point numbers.
        const double z = double(zoom) / 10;
        tiles.clear();
        test.map.setZoom(z);
        test.frontend.render(test.map);
        EXPECT_EQ(referenceTiles[z], tiles) << "zoom level " << z;
    }
}

TEST(Map, TEST_DISABLED_ON_CI(ContinuousRendering)) {
    util::RunLoop runLoop;
    ThreadPool threadPool { 4 };
    DefaultFileSource fileSource(":memory:", "test/fixtures/api/assets");
    float pixelRatio { 1 };

    using namespace std::chrono_literals;

    util::Timer emergencyShutoff;
    emergencyShutoff.start(10s, 0s, [&] {
        runLoop.stop();
        FAIL() << "Did not stop rendering";
    });

    util::Timer timer;

    HeadlessFrontend frontend(pixelRatio, fileSource, threadPool);

    StubMapObserver observer;
    observer.didFinishRenderingFrame = [&] (MapObserver::RenderMode) {
        // Start a timer that ends the test one second from now. If we are continuing to render
        // indefinitely, the timer will be constantly restarted and never trigger. Instead, the
        // emergency shutoff above will trigger, failing the test.
        timer.start(1s, 0s, [&] {
            runLoop.stop();
        });
    };

    Map map(frontend, observer, frontend.getSize(), pixelRatio, fileSource, threadPool, MapMode::Continuous);
    map.getStyle().loadJSON(util::read_file("test/fixtures/api/water.json"));

    runLoop.run();
}

TEST(Map, NoContentTiles) {
    MapTest<DefaultFileSource> test {":memory:", "."};

    using namespace std::chrono_literals;

    // Insert a 204 No Content response for the 0/0/0 tile
    Response response;
    response.noContent = true;
    response.expires = util::now() + 1h;
    test.fileSource.put(Resource::tile("http://example.com/{z}-{x}-{y}.vector.pbf", 1.0, 0, 0, 0,
                                       Tileset::Scheme::XYZ),
                        response);

    test.map.getStyle().loadJSON(R"STYLE({
      "version": 8,
      "name": "Water",
      "sources": {
        "mapbox": {
          "type": "vector",
          "tiles": ["http://example.com/{z}-{x}-{y}.vector.pbf"]
        }
      },
      "layers": [{
        "id": "background",
        "type": "background",
        "paint": {
          "background-color": "red"
        }
      }, {
        "id": "water",
        "type": "fill",
        "source": "mapbox",
        "source-layer": "water"
      }]
    })STYLE");

    test::checkImage("test/fixtures/map/nocontent",
                     test.frontend.render(test.map),
                     0.0015,
                     0.1);
}
