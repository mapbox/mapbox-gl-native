#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/fake_file_source.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/map/backend_scope.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/offscreen_view.hpp>
#include <mbgl/gl/context.hpp>
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

class MockBackend : public HeadlessBackend {
public:
    MockBackend()
            : HeadlessBackend(test::sharedDisplay() ) {
    }

    std::function<void()> callback;
    void invalidate() override {
        if (callback) {
            callback();
        }
    }
};

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
    
    std::function<void()> onWillStartLoadingMapCallback;
    std::function<void()> onDidFinishLoadingMapCallback;
    std::function<void()> didFailLoadingMapCallback;
    std::function<void()> didFinishLoadingStyleCallback;
};

struct MapTest {
    util::RunLoop runLoop;
    MockBackend backend;
    StubMapObserver observer;
    BackendScope scope { backend };
    OffscreenView view { backend.getContext() };
    StubFileSource fileSource;
    ThreadPool threadPool { 4 };
};

TEST(Map, LatLngBehavior) {
    MapTest test;
    Map map(test.backend, test.observer, test.view.getSize(), 1, test.fileSource, test.threadPool, MapMode::Still);

    map.setLatLngZoom({ 1, 1 }, 0);
    auto latLng1 = map.getLatLng();

    map.setLatLng({ 1, 1 });
    auto latLng2 = map.getLatLng();

    ASSERT_DOUBLE_EQ(latLng1.latitude(), latLng2.latitude());
    ASSERT_DOUBLE_EQ(latLng1.longitude(), latLng2.longitude());
}

TEST(Map, LatLngBoundsToCamera) {
    MapTest test;
    Map map(test.backend, test.observer, test.view.getSize(), 1, test.fileSource, test.threadPool, MapMode::Still);

    map.setLatLngZoom({ 40.712730, -74.005953 }, 16.0);

    LatLngBounds bounds = LatLngBounds::hull({15.68169,73.499857}, {53.560711, 134.77281});

    CameraOptions virtualCamera = map.cameraForLatLngBounds(bounds, {});
    ASSERT_TRUE(bounds.contains(*virtualCamera.center));
}

TEST(Map, CameraToLatLngBounds) {
    MapTest test;
    Map map(test.backend, test.observer, test.view.getSize(), 1, test.fileSource, test.threadPool, MapMode::Still);

    map.setLatLngZoom({ 45, 90 }, 16);

    LatLngBounds bounds = LatLngBounds::hull(
            map.latLngForPixel({}),
            map.latLngForPixel({ double(map.getSize().width), double(map.getSize().height) }));

    CameraOptions camera = map.getCameraOptions({});

    ASSERT_EQ(bounds, map.latLngBoundsForCamera(camera));

    // Map::cameraForLatLngBounds only sets zoom and center.
    CameraOptions virtualCamera = map.cameraForLatLngBounds(bounds, {});
    ASSERT_NEAR(*camera.zoom, *virtualCamera.zoom, 1e-7);
    ASSERT_NEAR(camera.center->latitude(), virtualCamera.center->latitude(), 1e-7);
    ASSERT_NEAR(camera.center->longitude(), virtualCamera.center->longitude(), 1e-7);
}

TEST(Map, Offline) {
    MapTest test;
    DefaultFileSource fileSource(":memory:", ".");

    auto expiredItem = [] (const std::string& path) {
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/map/offline/"s + path));
        response.expires = Timestamp{ Seconds(0) };
        return response;
    };

    const std::string prefix = "http://127.0.0.1:3000/";
    fileSource.put(Resource::style(prefix + "style.json"), expiredItem("style.json"));
    fileSource.put(Resource::source(prefix + "streets.json"), expiredItem("streets.json"));
    fileSource.put(Resource::spriteJSON(prefix + "sprite", 1.0), expiredItem("sprite.json"));
    fileSource.put(Resource::spriteImage(prefix + "sprite", 1.0), expiredItem("sprite.png"));
    fileSource.put(Resource::tile(prefix + "{z}-{x}-{y}.vector.pbf", 1.0, 0, 0, 0, Tileset::Scheme::XYZ), expiredItem("0-0-0.vector.pbf"));
    fileSource.put(Resource::glyphs(prefix + "{fontstack}/{range}.pbf", {{"Helvetica"}}, {0, 255}), expiredItem("glyph.pbf"));
    NetworkStatus::Set(NetworkStatus::Status::Offline);

    Map map(test.backend, test.observer, test.view.getSize(), 1, fileSource, test.threadPool, MapMode::Still);
    map.getStyle().loadURL(prefix + "style.json");

    test::checkImage("test/fixtures/map/offline",
                     test::render(map, test.view),
                     0.0015,
                     0.1);

    NetworkStatus::Set(NetworkStatus::Status::Online);
}

TEST(Map, SetStyleInvalidJSON) {
    MapTest test;

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    bool fail = false;
    test.observer.didFailLoadingMapCallback = [&]() {
        fail = true;
    };

    {
        Map map(test.backend, test.observer, test.view.getSize(), 1, test.fileSource, test.threadPool,
                MapMode::Still);
        map.getStyle().loadJSON("invalid");
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
    MapTest test;

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

    Map map(test.backend, test.observer, test.view.getSize(), 1, test.fileSource, test.threadPool, MapMode::Still);
    map.getStyle().loadURL("mapbox://bar");

    test.runLoop.run();
}

TEST(Map, DoubleStyleLoad) {
    MapTest test;

    Map map(test.backend, test.observer, test.view.getSize(), 1, test.fileSource, test.threadPool, MapMode::Still);
    map.getStyle().loadJSON("");
    map.getStyle().loadJSON("");
}

TEST(Map, StyleFresh) {
    // The map should not revalidate fresh styles.

    MapTest test;
    FakeFileSource fileSource;

    Map map(test.backend, test.observer, test.view.getSize(), 1, fileSource, test.threadPool, MapMode::Still);
    map.getStyle().loadURL("mapbox://styles/test");
    EXPECT_EQ(1u, fileSource.requests.size());

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/empty.json"));
    response.expires = Timestamp::max();

    fileSource.respond(Resource::Style, response);
    EXPECT_EQ(0u, fileSource.requests.size());
}

TEST(Map, StyleExpired) {
    // The map should allow expired styles to be revalidated, so long as no mutations are made.

    using namespace std::chrono_literals;

    MapTest test;
    FakeFileSource fileSource;

    Map map(test.backend, test.observer, test.view.getSize(), 1, fileSource, test.threadPool, MapMode::Still);
    map.getStyle().loadURL("mapbox://styles/test");
    EXPECT_EQ(1u, fileSource.requests.size());

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/empty.json"));
    response.expires = util::now() - 1h;

    fileSource.respond(Resource::Style, response);
    EXPECT_EQ(1u, fileSource.requests.size());

    map.getStyle().addLayer(std::make_unique<style::BackgroundLayer>("bg"));
    EXPECT_EQ(1u, fileSource.requests.size());

    fileSource.respond(Resource::Style, response);
    EXPECT_EQ(0u, fileSource.requests.size());
    EXPECT_NE(nullptr, map.getStyle().getLayer("bg"));
}

TEST(Map, StyleExpiredWithAnnotations) {
    // Adding an annotation should not prevent revalidation of an expired style.

    using namespace std::chrono_literals;

    MapTest test;
    FakeFileSource fileSource;

    Map map(test.backend, test.observer, test.view.getSize(), 1, fileSource, test.threadPool, MapMode::Still);
    map.getStyle().loadURL("mapbox://styles/test");
    EXPECT_EQ(1u, fileSource.requests.size());

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/empty.json"));
    response.expires = util::now() - 1h;

    fileSource.respond(Resource::Style, response);
    EXPECT_EQ(1u, fileSource.requests.size());

    map.addAnnotation(LineAnnotation { LineString<double> {{ { 0, 0 }, { 10, 10 } }} });
    EXPECT_EQ(1u, fileSource.requests.size());

    fileSource.respond(Resource::Style, response);
    EXPECT_EQ(1u, fileSource.requests.size());
}

TEST(Map, StyleExpiredWithRender) {
    // Rendering should not prevent revalidation of an expired style.

    using namespace std::chrono_literals;

    MapTest test;
    FakeFileSource fileSource;

    Map map(test.backend, test.observer, test.view.getSize(), 1, fileSource, test.threadPool, MapMode::Still);
    map.getStyle().loadURL("mapbox://styles/test");
    EXPECT_EQ(1u, fileSource.requests.size());

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/empty.json"));
    response.expires = util::now() - 1h;

    fileSource.respond(Resource::Style, response);
    EXPECT_EQ(1u, fileSource.requests.size());

    map.render(test.view);
    EXPECT_EQ(1u, fileSource.requests.size());

    fileSource.respond(Resource::Style, response);
    EXPECT_EQ(1u, fileSource.requests.size());
}

TEST(Map, StyleEarlyMutation) {
    // An early mutation should not prevent the initial style load.

    MapTest test;
    FakeFileSource fileSource;

    Map map(test.backend, test.observer, test.view.getSize(), 1, fileSource, test.threadPool, MapMode::Still);
    map.getStyle().loadURL("mapbox://styles/test");
    map.getStyle().addLayer(std::make_unique<style::BackgroundLayer>("bg"));

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/water.json"));
    fileSource.respond(Resource::Style, response);

    EXPECT_EQ(0u, fileSource.requests.size());
    EXPECT_NE(nullptr, map.getStyle().getLayer("water"));
}

TEST(Map, MapLoadingSignal) {
    MapTest test;
    Map map(test.backend, test.observer, test.view.getSize(), 1, test.fileSource, test.threadPool, MapMode::Still);

    bool emitted = false;
    test.observer.onWillStartLoadingMapCallback = [&]() {
        emitted = true;
    };
    map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    EXPECT_TRUE(emitted);
}

TEST(Map, MapLoadedSignal) {
    MapTest test;
    Map map(test.backend, test.observer, test.view.getSize(), 1, test.fileSource, test.threadPool, MapMode::Continuous);

    test.observer.onDidFinishLoadingMapCallback = [&]() {
        test.runLoop.stop();
    };

    test.backend.callback = [&]() {
        map.render(test.view);
    };

    map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.runLoop.run();
}

TEST(Map, StyleLoadedSignal) {
    MapTest test;
    Map map(test.backend, test.observer, test.view.getSize(), 1, test.fileSource, test.threadPool, MapMode::Still);

    // The map should emit a signal on style loaded
    bool emitted = false;
    test.observer.didFinishLoadingStyleCallback = [&]() {
        emitted = true;
    };
    map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    EXPECT_TRUE(emitted);

    // But not when the style couldn't be parsed
    emitted = false;
    map.getStyle().loadJSON("invalid");
    EXPECT_FALSE(emitted);
}

// Test for https://github.com/mapbox/mapbox-gl-native/issues/7902
TEST(Map, TEST_REQUIRES_SERVER(StyleNetworkErrorRetry)) {
    MapTest test;
    OnlineFileSource fileSource;

    Map map(test.backend, test.observer, test.view.getSize(), 1, fileSource, test.threadPool, MapMode::Still);
    map.getStyle().loadURL("http://127.0.0.1:3000/style-fail-once-500");

    test.observer.didFinishLoadingStyleCallback = [&]() {
        test.runLoop.stop();
    };

    test.runLoop.run();
}

TEST(Map, TEST_REQUIRES_SERVER(StyleNotFound)) {
    MapTest test;
    OnlineFileSource fileSource;

    Map map(test.backend, test.observer, test.view.getSize(), 1, fileSource, test.threadPool, MapMode::Still);
    map.getStyle().loadURL("http://127.0.0.1:3000/style-fail-once-404");

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
    map.getStyle().loadURL("http://127.0.0.1:3000/style-fail-once-404-cache");
    test.runLoop.run();
}

TEST(Map, AddLayer) {
    MapTest test;

    Map map(test.backend, test.observer, test.view.getSize(), 1, test.fileSource, test.threadPool, MapMode::Still);
    map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));

    auto layer = std::make_unique<BackgroundLayer>("background");
    layer->setBackgroundColor({ { 1, 0, 0, 1 } });
    map.getStyle().addLayer(std::move(layer));

    test::checkImage("test/fixtures/map/add_layer", test::render(map, test.view));
}

TEST(Map, WithoutVAOExtension) {
    MapTest test;

    test.backend.getContext().disableVAOExtension = true;

    DefaultFileSource fileSource(":memory:", "test/fixtures/api/assets");

    Map map(test.backend, test.observer, test.view.getSize(), 1, fileSource, test.threadPool, MapMode::Still);
    map.getStyle().loadJSON(util::read_file("test/fixtures/api/water.json"));

    test::checkImage("test/fixtures/map/no_vao", test::render(map, test.view), 0.002);
}

TEST(Map, RemoveLayer) {
    MapTest test;

    Map map(test.backend, test.observer, test.view.getSize(), 1, test.fileSource, test.threadPool, MapMode::Still);
    map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));

    auto layer = std::make_unique<BackgroundLayer>("background");
    layer->setBackgroundColor({{ 1, 0, 0, 1 }});
    map.getStyle().addLayer(std::move(layer));
    map.getStyle().removeLayer("background");

    test::checkImage("test/fixtures/map/remove_layer", test::render(map, test.view));
}

TEST(Map, DisabledSources) {
    MapTest test;

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

    Map map(test.backend, test.observer, test.view.getSize(), 1, test.fileSource, test.threadPool, MapMode::Still);
    map.setZoom(1);

    // This stylesheet has two raster layers, one that starts at zoom 1, the other at zoom 0.
    // We first render a map at zoom level 1, which should show both layers (both are "visible" due
    // to an opacity of 0.5). Then, we are zooming back out to a zoom level of 0.5 and rerender.
    // The "raster1" layer should not be visible anymore since it has minzoom 1, while "raster2"
    // should still be there. Both layers have a distinct color through "raster-hue-rotate".
    map.getStyle().loadJSON(R"STYLE(
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

    test::checkImage("test/fixtures/map/disabled_layers/first", test::render(map, test.view));
    map.setZoom(0.5);
    test::checkImage("test/fixtures/map/disabled_layers/second", test::render(map, test.view));
}

TEST(Map, DontLoadUnneededTiles) {
    MapTest test;

    Map map(test.backend, test.observer, test.view.getSize(), 1, test.fileSource, test.threadPool, MapMode::Still);
    map.getStyle().loadJSON(R"STYLE({
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
        map.setZoom(z);
        test::render(map, test.view);
        EXPECT_EQ(referenceTiles[z], tiles) << "zoom level " << z;
    }
}

TEST(Map, TEST_DISABLED_ON_CI(ContinuousRendering)) {
    util::RunLoop runLoop;
    MockBackend backend;
    BackendScope scope { backend };
    OffscreenView view { backend.getContext() };
    ThreadPool threadPool { 4 };
    DefaultFileSource fileSource(":memory:", "test/fixtures/api/assets");
    Map map(backend, MapObserver::nullObserver(), view.getSize(), 1, fileSource, threadPool, MapMode::Continuous);

    using namespace std::chrono_literals;

    util::Timer emergencyShutoff;
    emergencyShutoff.start(10s, 0s, [&] {
        util::RunLoop::Get()->stop();
        FAIL() << "Did not stop rendering";
    });

    util::Timer timer;
    util::AsyncTask render{[&] {
        if (map.isFullyLoaded()) {
            // Abort the test after 1 second after the map loading fully. Note that a "fully loaded
            // map" doesn't mean that we won't render anymore: we could still render fade in/fade
            // out or other animations.
            // If we are continuing to render indefinitely, the emergency shutoff above will trigger
            // and the test will fail since the regular time will be constantly reset.
            timer.start(1s, 0s, [&] {
                util::RunLoop::Get()->stop();
            });
        }

        BackendScope scope2(backend);
        map.render(view);
    }};

    backend.callback = [&] {
        render.send();
    };

    map.getStyle().loadJSON(util::read_file("test/fixtures/api/water.json"));
    util::RunLoop::Get()->run();
}
