#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/stub_map_observer.hpp>
#include <mbgl/test/fake_file_source.hpp>
#include <mbgl/test/fixture_log_observer.hpp>
#include <mbgl/test/map_adapter.hpp>

#include <mbgl/map/map_options.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/storage/resource_options.hpp>
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
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/util/color.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace std::literals::string_literals;

template <class FileSource = StubFileSource>
class MapTest {
public:
    util::RunLoop runLoop;
    std::shared_ptr<FileSource> fileSource;
    StubMapObserver observer;
    HeadlessFrontend frontend;
    MapAdapter map;

    MapTest(float pixelRatio = 1, MapMode mode = MapMode::Static)
        : fileSource(std::make_shared<FileSource>())
        , frontend(pixelRatio)
        , map(frontend, observer, fileSource,
              MapOptions().withMapMode(mode).withSize(frontend.getSize()).withPixelRatio(pixelRatio)) {}

    template <typename T = FileSource>
    MapTest(const std::string& cachePath, const std::string& assetPath,
            float pixelRatio = 1, MapMode mode = MapMode::Static,
            typename std::enable_if<std::is_same<T, DefaultFileSource>::value>::type* = nullptr)
            : fileSource(std::make_shared<T>(cachePath, assetPath))
            , frontend(pixelRatio)
            , map(frontend, observer, fileSource,
                  MapOptions().withMapMode(mode).withSize(frontend.getSize()).withPixelRatio(pixelRatio)) {}
};

TEST(Map, RendererState) {
    MapTest<> test;

    // Map hasn't notified the frontend about an update yet.
    CameraOptions nullOptions;
    ASSERT_EQ(test.frontend.getCameraOptions(), nullOptions);

    LatLng coordinate { 1, 1 };
    double zoom = 12.0;
    double pitchInDegrees = 45.0;
    double bearingInDegrees = 30.0;

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.jumpTo(CameraOptions().withCenter(coordinate).withZoom(zoom).withPitch(pitchInDegrees).withBearing(bearingInDegrees));

    test.runLoop.runOnce();
    test.frontend.render(test.map);

    // RendererState::getCameraOptions
    const CameraOptions& options = test.frontend.getCameraOptions();
    EXPECT_NEAR(options.center->latitude(), coordinate.latitude(), 1e-7);
    EXPECT_NEAR(options.center->longitude(), coordinate.longitude(), 1e-7);
    ASSERT_DOUBLE_EQ(*options.zoom, zoom);
    ASSERT_DOUBLE_EQ(*options.pitch, pitchInDegrees);
    EXPECT_NEAR(*options.bearing, bearingInDegrees, 1e-7);

    {
        const LatLng& latLng = test.frontend.latLngForPixel(ScreenCoordinate { 0, 0 });
        const ScreenCoordinate& point = test.frontend.pixelForLatLng(coordinate);
        EXPECT_NEAR(coordinate.latitude(), latLng.latitude(), 1e-1);
        EXPECT_NEAR(coordinate.longitude(), latLng.longitude(), 1e-1);
        const Size size = test.map.getMapOptions().size();
        EXPECT_NEAR(point.x, size.width / 2.0, 1e-7);
        EXPECT_NEAR(point.y, size.height / 2.0, 1e-7);
    }

    // RendererState::hasImage
    test.map.getStyle().addImage(std::make_unique<style::Image>("default_marker", decodeImage(util::read_file("test/fixtures/sprites/default_marker.png")), 1.0));

    // The frontend has not yet been notified about the newly-added image.
    EXPECT_FALSE(test.frontend.hasImage("default_marker"));

    test.runLoop.runOnce();
    test.frontend.render(test.map);

    EXPECT_TRUE(test.frontend.hasImage("default_marker"));

    // RendererState::hasSource
    auto source = std::make_unique<GeoJSONSource>("GeoJSONSource");
    source->setGeoJSON( Geometry<double>{ Point<double>{ 0, 0 } } );
    test.map.getStyle().addSource(std::move(source));

    // The frontend has not yet been notified about the newly-added source.
    EXPECT_FALSE(test.frontend.hasSource("GeoJSONSource"));

    test.runLoop.runOnce();
    test.frontend.render(test.map);

    EXPECT_TRUE(test.frontend.hasSource("GeoJSONSource"));

    // RendererState::hasLayer
    test.map.getStyle().addLayer(std::make_unique<SymbolLayer>("SymbolLayer", "GeoJSONSource"));

    // The frontend has not yet been notified about the newly-added source.
    EXPECT_FALSE(test.frontend.hasLayer("SymbolLayer"));

    test.runLoop.runOnce();
    test.frontend.render(test.map);

    EXPECT_TRUE(test.frontend.hasLayer("SymbolLayer"));
}

TEST(Map, LatLngBehavior) {
    MapTest<> test;

    test.map.jumpTo(CameraOptions().withCenter(LatLng { 1.0, 1.0 }).withZoom(0.0));
    auto latLng1 = *test.map.getCameraOptions().center;

    test.map.jumpTo(CameraOptions().withCenter(LatLng { 1.0, 1.0 }));
    auto latLng2 = *test.map.getCameraOptions().center;

    ASSERT_DOUBLE_EQ(latLng1.latitude(), latLng2.latitude());
    ASSERT_DOUBLE_EQ(latLng1.longitude(), latLng2.longitude());
}

TEST(Map, LatLngBoundsToCamera) {
    MapTest<> test;

    test.map.jumpTo(CameraOptions().withCenter(LatLng { 40.712730, -74.005953 }).withZoom(16.0));

    LatLngBounds bounds = LatLngBounds::hull({15.68169,73.499857}, {53.560711, 134.77281});

    CameraOptions virtualCamera = test.map.cameraForLatLngBounds(bounds, {});
    ASSERT_TRUE(bounds.contains(*virtualCamera.center));
    EXPECT_NEAR(*virtualCamera.zoom, 1.55467, 1e-5);
}

TEST(Map, LatLngBoundsToCameraWithExcessivePadding) {
    MapTest<> test;

    test.map.jumpTo(CameraOptions().withCenter(LatLng { 40.712730, -74.005953 }).withZoom(16.0));

    LatLngBounds bounds = LatLngBounds::hull({15.68169,73.499857}, {53.560711, 134.77281});

    CameraOptions virtualCamera = test.map.cameraForLatLngBounds(bounds, {500, 0, 1200, 0});
    ASSERT_TRUE(bounds.contains(*virtualCamera.center));
    EXPECT_NEAR(*virtualCamera.zoom, 16.0, 1e-5);
}

TEST(Map, LatLngBoundsToCameraWithBearing) {
    MapTest<> test;

    test.map.jumpTo(CameraOptions().withCenter(LatLng { 40.712730, -74.005953 }).withZoom(16.0));

    LatLngBounds bounds = LatLngBounds::hull({15.68169,73.499857}, {53.560711, 134.77281});

    CameraOptions virtualCamera = test.map.cameraForLatLngBounds(bounds, {}, 35.0);
    ASSERT_TRUE(bounds.contains(*virtualCamera.center));
    EXPECT_NEAR(*virtualCamera.zoom, 1.21385, 1e-5);
    EXPECT_NEAR(virtualCamera.bearing.value_or(0), 35.0, 1e-5);
}

TEST(Map, LatLngBoundsToCameraWithBearingAndPitch) {
    MapTest<> test;
    
    test.map.jumpTo(CameraOptions().withCenter(LatLng { 40.712730, -74.005953 }).withZoom(16.0));
    
    LatLngBounds bounds = LatLngBounds::hull({15.68169,73.499857}, {53.560711, 134.77281});
    
    CameraOptions virtualCamera = test.map.cameraForLatLngBounds(bounds, {}, 35, 20);
    ASSERT_TRUE(bounds.contains(*virtualCamera.center));
    EXPECT_NEAR(*virtualCamera.zoom, 13.66272, 1e-5);
    ASSERT_DOUBLE_EQ(*virtualCamera.pitch, 20.0);
    EXPECT_NEAR(virtualCamera.bearing.value_or(0), 35.0, 1e-5);
}

TEST(Map, LatLngsToCamera) {
    MapTest<> test;

    std::vector<LatLng> latLngs{{ 40.712730, 74.005953 }, {15.68169,73.499857}, {30.82678, 83.4082}};

    CameraOptions virtualCamera = test.map.cameraForLatLngs(latLngs, {}, 23.0);
    EXPECT_NEAR(virtualCamera.bearing.value_or(0), 23.0, 1e-5);
    EXPECT_NEAR(virtualCamera.zoom.value_or(0), 2.75434, 1e-5);
    EXPECT_NEAR(virtualCamera.center->latitude(), 28.49288, 1e-5);
    EXPECT_NEAR(virtualCamera.center->longitude(), 74.97437, 1e-5);
}

TEST(Map, LatLngsToCameraWithBearingAndPitch) {
    MapTest<> test;
    
    std::vector<LatLng> latLngs{{ 40.712730, 74.005953 }, {15.68169,73.499857}, {30.82678, 83.4082}};
    
    CameraOptions virtualCamera = test.map.cameraForLatLngs(latLngs, {}, 23, 20);
    EXPECT_NEAR(virtualCamera.bearing.value_or(0), 23.0, 1e-5);
    EXPECT_NEAR(virtualCamera.zoom.value_or(0), 3.04378, 1e-5);
    EXPECT_NEAR(virtualCamera.center->latitude(), 28.53718, 1e-5);
    EXPECT_NEAR(virtualCamera.center->longitude(), 74.31746, 1e-5);
    ASSERT_DOUBLE_EQ(*virtualCamera.pitch, 20.0);
}


TEST(Map, CameraToLatLngBounds) {
    MapTest<> test;

    test.map.jumpTo(CameraOptions().withCenter(LatLng { 45, 90 }).withZoom(16.0));

    const Size size = test.map.getMapOptions().size();
    LatLngBounds bounds = LatLngBounds::hull(
            test.map.latLngForPixel({}),
            test.map.latLngForPixel({ double(size.width), double(size.height) }));

    CameraOptions camera = test.map.getCameraOptions();

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
    test.fileSource->put(Resource::style(prefix + "style.json"), expiredItem("style.json"));
    test.fileSource->put(Resource::source(prefix + "streets.json"), expiredItem("streets.json"));
    test.fileSource->put(Resource::spriteJSON(prefix + "sprite", 1.0), expiredItem("sprite.json"));
    test.fileSource->put(Resource::spriteImage(prefix + "sprite", 1.0), expiredItem("sprite.png"));
    test.fileSource->put(Resource::tile(prefix + "{z}-{x}-{y}.vector.pbf", 1.0, 0, 0, 0, Tileset::Scheme::XYZ), expiredItem("0-0-0.vector.pbf"));
    test.fileSource->put(Resource::glyphs(prefix + "{fontstack}/{range}.pbf", {{"Helvetica"}}, {0, 255}), expiredItem("glyph.pbf"));
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
    CameraOptions camera = test.map.getCameraOptions();
    EXPECT_DOUBLE_EQ(*camera.zoom, 0.0);
    EXPECT_DOUBLE_EQ(*camera.pitch, 0.0);
    EXPECT_DOUBLE_EQ(*camera.bearing, 0.0);
    EXPECT_EQ(*camera.center, LatLng {});

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty-zoomed.json"));
    camera = test.map.getCameraOptions();
    EXPECT_DOUBLE_EQ(*camera.zoom, 0.0);

    test.map.jumpTo(test.map.getStyle().getDefaultCamera());
    camera = test.map.getCameraOptions();
    EXPECT_DOUBLE_EQ(*camera.zoom, 0.5);
}

TEST(Map, ProjectionMode) {
    MapTest<> test;

    test.map.setProjectionMode(ProjectionMode().withAxonometric(true).withXSkew(1.0).withYSkew(0.0));
    auto options = test.map.getProjectionMode();

    EXPECT_TRUE(*options.axonometric);
    EXPECT_EQ(*options.xSkew, 1.0);
    EXPECT_EQ(*options.ySkew, 0.0);
}

TEST(Map, BoundOptions) {
    MapTest<> test;

    LatLngBounds llb = LatLngBounds::hull({-10, -10}, {10, 10});
    test.map.setBounds(BoundOptions().withMinZoom(4).withMaxZoom(10).withLatLngBounds(llb));
    auto bounds = test.map.getBounds();

    EXPECT_EQ(*bounds.minZoom, 4);
    EXPECT_EQ(*bounds.maxZoom, 10);
    EXPECT_EQ(*bounds.bounds, llb);
}

TEST(Map, DefaultBoundOptions) {
    MapTest<> test;

    auto bounds = test.map.getBounds();

    EXPECT_TRUE(bounds.minZoom);
    EXPECT_TRUE(bounds.maxZoom);
    EXPECT_TRUE(bounds.bounds);

    EXPECT_EQ(*bounds.minZoom, util::MIN_ZOOM);
    EXPECT_EQ(*bounds.maxZoom, util::DEFAULT_MAX_ZOOM);
    EXPECT_EQ(*bounds.bounds, LatLngBounds::unbounded());
}

TEST(Map, MapOptions) {
    float pixelRatio { 2 };
    MapTest<> test { pixelRatio, MapMode::Continuous };

    test.map.setNorthOrientation(NorthOrientation::Rightwards);
    test.map.setConstrainMode(ConstrainMode::None);
    test.map.setViewportMode(ViewportMode::FlippedY);
    Size size = { 512, 512 };
    test.map.setSize(size);

    auto options = test.map.getMapOptions();
    EXPECT_EQ(options.mapMode(), MapMode::Continuous);
    EXPECT_EQ(options.viewportMode(), ViewportMode::FlippedY);
    EXPECT_EQ(options.constrainMode(), ConstrainMode::None);
    EXPECT_EQ(options.northOrientation(), NorthOrientation::Rightwards);
    EXPECT_EQ(options.size(), size);
    EXPECT_EQ(options.pixelRatio(), pixelRatio);
}

TEST(Map, DefaultMapOptions) {
    MapTest<> test;

    auto options = test.map.getMapOptions();
    EXPECT_EQ(options.mapMode(), MapMode::Static);
    EXPECT_EQ(options.viewportMode(), ViewportMode::Default);
    EXPECT_EQ(options.constrainMode(), ConstrainMode::HeightOnly);
    EXPECT_EQ(options.northOrientation(), NorthOrientation::Upwards);
    EXPECT_TRUE(options.crossSourceCollisions());
    EXPECT_EQ(options.size().width, 256);
    EXPECT_EQ(options.size().height, 256);
    EXPECT_EQ(options.pixelRatio(), 1);
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
    auto flo = static_cast<FixtureLogObserver*>(observer.get());
    EXPECT_EQ(1u, flo->count({ EventSeverity::Error, Event::ParseStyle, -1,
        "Failed to parse style: Invalid value. at offset 0" }));
    auto unchecked = flo->unchecked();
    EXPECT_TRUE(unchecked.empty()) << unchecked;
}

TEST(Map, SetStyleInvalidURL) {
    MapTest<> test;

    test.fileSource->styleResponse = [] (const Resource&) {
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
    EXPECT_EQ(1u, test.fileSource->requests.size());

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/empty.json"));
    response.expires = Timestamp::max();

    test.fileSource->respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource->requests.size());
}

TEST(Map, StyleExpired) {
    // The map should allow expired styles to be revalidated until we get a fresh style.

    using namespace std::chrono_literals;

    MapTest<FakeFileSource> test;

    test.map.getStyle().loadURL("mapbox://styles/test");
    EXPECT_EQ(1u, test.fileSource->requests.size());

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/empty.json"));
    response.expires = util::now() - 1h;

    test.fileSource->respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource->requests.size());

    // Mutate layer. From now on, sending a response to the style won't overwrite it anymore, but
    // we should continue to wait for a fresh response.
    test.map.getStyle().addLayer(std::make_unique<style::BackgroundLayer>("bg"));
    EXPECT_EQ(1u, test.fileSource->requests.size());

    // Send another expired response, and confirm that we didn't overwrite the style, but continue
    // to wait for a fresh response.
    test.fileSource->respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource->requests.size());
    EXPECT_NE(nullptr, test.map.getStyle().getLayer("bg"));

    // Send a fresh response, and confirm that we didn't overwrite the style, but continue to wait
    // for a fresh response.
    response.expires = util::now() + 1h;
    test.fileSource->respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource->requests.size());
    EXPECT_NE(nullptr, test.map.getStyle().getLayer("bg"));
}

TEST(Map, StyleExpiredWithAnnotations) {
    // Adding an annotation should not prevent revalidation of an expired style.

    using namespace std::chrono_literals;

    MapTest<FakeFileSource> test;

    test.map.getStyle().loadURL("mapbox://styles/test");
    EXPECT_EQ(1u, test.fileSource->requests.size());

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/empty.json"));
    response.expires = util::now() - 1h;

    test.fileSource->respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource->requests.size());

    test.map.addAnnotation(LineAnnotation { LineString<double> {{ { 0, 0 }, { 10, 10 } }} });
    EXPECT_EQ(1u, test.fileSource->requests.size());

    test.fileSource->respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource->requests.size());
}

TEST(Map, StyleExpiredWithRender) {
    // Rendering should not prevent revalidation of an expired style.

    using namespace std::chrono_literals;

    MapTest<FakeFileSource> test;

    test.map.getStyle().loadURL("mapbox://styles/test");
    EXPECT_EQ(1u, test.fileSource->requests.size());

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/empty.json"));
    response.expires = util::now() - 1h;

    test.fileSource->respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource->requests.size());

    test.frontend.render(test.map);
    EXPECT_EQ(1u, test.fileSource->requests.size());

    test.fileSource->respond(Resource::Style, response);
    EXPECT_EQ(1u, test.fileSource->requests.size());
}

TEST(Map, StyleEarlyMutation) {
    // An early mutation should not prevent the initial style load.

    MapTest<FakeFileSource> test;

    test.map.getStyle().loadURL("mapbox://styles/test");
    test.map.getStyle().addLayer(std::make_unique<style::BackgroundLayer>("bg"));

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/water.json"));
    test.fileSource->respond(Resource::Style, response);

    EXPECT_EQ(1u, test.fileSource->requests.size());
    EXPECT_NE(nullptr, test.map.getStyle().getLayer("water"));
}

TEST(Map, MapLoadingSignal) {
    MapTest<> test;

    bool emitted = false;
    test.observer.willStartLoadingMapCallback = [&]() {
        emitted = true;
    };
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    EXPECT_TRUE(emitted);
}

TEST(Map, MapLoadedSignal) {
    MapTest<> test { 1, MapMode::Continuous };

    test.observer.didFinishLoadingMapCallback = [&]() {
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
    if (gfx::Backend::GetType() != gfx::Backend::Type::OpenGL) {
        return;
    }

    MapTest<DefaultFileSource> test { ":memory:", "test/fixtures/api/assets" };

    gfx::BackendScope scope { *test.frontend.getBackend() };
    static_cast<gl::Context&>(test.frontend.getBackend()->getContext()).disableVAOExtension = true;

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
    test.fileSource->response = [] (const Resource& res) -> optional<Response> {
        if (res.url == "asset://tile.png") {
            Response response;
            response.data = std::make_shared<std::string>(
                util::read_file("test/fixtures/map/disabled_layers/tile.png"));
            return {std::move(response)};
        }
        return {};
    };

    test.map.jumpTo(CameraOptions().withZoom(1.0));

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
    test.map.jumpTo(CameraOptions().withZoom(0.5));
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

    test.fileSource->tileResponse = [&](const Resource& rsc) {
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
        test.map.jumpTo(CameraOptions().withZoom(z));
        test.frontend.render(test.map);
        EXPECT_EQ(referenceTiles[z], tiles) << "zoom level " << z;
    }
}

TEST(Map, TEST_DISABLED_ON_CI(ContinuousRendering)) {
    util::RunLoop runLoop;

    using namespace std::chrono_literals;

    util::Timer emergencyShutoff;
    emergencyShutoff.start(10s, 0s, [&] {
        runLoop.stop();
        FAIL() << "Did not stop rendering";
    });

    util::Timer timer;

    HeadlessFrontend frontend(1);

    StubMapObserver observer;
    observer.didFinishRenderingFrameCallback = [&] (MapObserver::RenderMode) {
        // Start a timer that ends the test one second from now. If we are continuing to render
        // indefinitely, the timer will be constantly restarted and never trigger. Instead, the
        // emergency shutoff above will trigger, failing the test.
        timer.start(1s, 0s, [&] {
            runLoop.stop();
        });
    };

    Map map(frontend, observer,
            MapOptions().withMapMode(MapMode::Continuous).withSize(frontend.getSize()),
            ResourceOptions().withCachePath(":memory:").withAssetPath("test/fixtures/api/assets"));
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
    test.fileSource->put(Resource::tile("http://example.com/{z}-{x}-{y}.vector.pbf", 1.0, 0, 0, 0,
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

// https://github.com/mapbox/mapbox-gl-native/issues/12432
TEST(Map, Issue12432) {
    MapTest<> test { 1, MapMode::Continuous };

    test.fileSource->tileResponse = [&](const Resource&) {
        Response result;
        result.data = std::make_shared<std::string>(util::read_file("test/fixtures/map/issue12432/0-0-0.mvt"));
        return result;
    };

    test.map.getStyle().loadJSON(R"STYLE({
      "version": 8,
      "sources": {
        "mapbox": {
          "type": "vector",
          "tiles": ["http://example.com/{z}-{x}-{y}.vector.pbf"]
        }
      },
      "layers": [{
        "id": "water",
        "type": "fill",
        "source": "mapbox",
        "source-layer": "water"
      }]
    })STYLE");

    test.observer.didFinishLoadingMapCallback = [&]() {
        test.map.getStyle().loadJSON(R"STYLE({
          "version": 8,
          "sources": {
            "mapbox": {
              "type": "vector",
              "tiles": ["http://example.com/{z}-{x}-{y}.vector.pbf"]
            }
          },
          "layers": [{
            "id": "water",
            "type": "line",
            "source": "mapbox",
            "source-layer": "water"
          }]
        })STYLE");

        test.runLoop.stop();
    };

    test.runLoop.run();
}

// https://github.com/mapbox/mapbox-gl-native/issues/15216
TEST(Map, Issue15216) {
    MapTest<> test { 1.0f,  MapMode::Continuous };
    test.map.getStyle().addSource(std::make_unique<ImageSource>("ImageSource", std::array<LatLng, 4>()));
    test.map.getStyle().addLayer(std::make_unique<RasterLayer>("RasterLayer", "ImageSource"));
    // Passes, if there is no assertion hit.
    test.runLoop.runOnce();
}