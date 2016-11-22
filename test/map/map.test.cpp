#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/fake_file_source.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/thread_pool.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/util/color.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace std::literals::string_literals;

struct MapTest {
    util::RunLoop runLoop;
    std::shared_ptr<HeadlessDisplay> display { std::make_shared<mbgl::HeadlessDisplay>() };
    HeadlessView view { display, 1 };
    StubFileSource fileSource;
    ThreadPool threadPool { 4 };
};

TEST(Map, LatLngBehavior) {
    MapTest test;
    Map map(test.view, test.fileSource, test.threadPool, MapMode::Still);

    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));

    map.setLatLngZoom({ 1, 1 }, 0);
    auto latLng1 = map.getLatLng();

    map.setLatLng({ 1, 1 });
    auto latLng2 = map.getLatLng();

    ASSERT_DOUBLE_EQ(latLng1.latitude, latLng2.latitude);
    ASSERT_DOUBLE_EQ(latLng1.longitude, latLng2.longitude);
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

    Map map(test.view, fileSource, test.threadPool, MapMode::Still);
    map.setStyleURL(prefix + "style.json");

    test::checkImage("test/fixtures/map/offline",
                     test::render(map),
                     0.0015,
                     0.1);

    NetworkStatus::Set(NetworkStatus::Status::Online);
}

TEST(Map, SetStyleInvalidJSON) {
    MapTest test;

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    bool fail = false;
    test.view.setMapChangeCallback([&](MapChange change) {
        if (change == mbgl::MapChangeDidFailLoadingMap) {
            fail = true;
        }
    });

    {
        Map map(test.view, test.fileSource, test.threadPool, MapMode::Still);
        map.setStyleJSON("invalid");
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

    test.view.setMapChangeCallback([&](MapChange change) {
        if (change == mbgl::MapChangeDidFailLoadingMap) {
            test.runLoop.stop();
        }
    });

    Map map(test.view, test.fileSource, test.threadPool, MapMode::Still);
    map.setStyleURL("mapbox://bar");

    test.runLoop.run();
}

TEST(Map, DoubleStyleLoad) {
    MapTest test;

    Map map(test.view, test.fileSource, test.threadPool, MapMode::Still);
    map.setStyleJSON("");
    map.setStyleJSON("");
}

TEST(Map, StyleFresh) {
    // The map should not revalidate fresh styles.

    MapTest test;
    FakeFileSource fileSource;

    Map map(test.view, fileSource, test.threadPool, MapMode::Still);
    map.setStyleURL("mapbox://styles/test");
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

    Map map(test.view, fileSource, test.threadPool, MapMode::Still);
    map.setStyleURL("mapbox://styles/test");
    EXPECT_EQ(1u, fileSource.requests.size());

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/empty.json"));
    response.expires = util::now() - 1h;

    fileSource.respond(Resource::Style, response);
    EXPECT_EQ(1u, fileSource.requests.size());

    map.addLayer(std::make_unique<style::BackgroundLayer>("bg"));
    EXPECT_EQ(1u, fileSource.requests.size());

    fileSource.respond(Resource::Style, response);
    EXPECT_EQ(0u, fileSource.requests.size());
    EXPECT_NE(nullptr, map.getLayer("bg"));
}

TEST(Map, StyleExpiredWithAnnotations) {
    // Adding an annotation should not prevent revalidation of an expired style.

    using namespace std::chrono_literals;

    MapTest test;
    FakeFileSource fileSource;

    Map map(test.view, fileSource, test.threadPool, MapMode::Still);
    map.setStyleURL("mapbox://styles/test");
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

TEST(Map, StyleEarlyMutation) {
    // An early mutation should not prevent the initial style load.

    MapTest test;
    FakeFileSource fileSource;

    Map map(test.view, fileSource, test.threadPool, MapMode::Still);
    map.setStyleURL("mapbox://styles/test");
    map.addLayer(std::make_unique<style::BackgroundLayer>("bg"));

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/water.json"));
    fileSource.respond(Resource::Style, response);

    EXPECT_EQ(0u, fileSource.requests.size());
    EXPECT_NE(nullptr, map.getLayer("water"));
}

TEST(Map, StyleLoadedSignal) {
    MapTest test;
    Map map(test.view, test.fileSource, test.threadPool, MapMode::Still);
    
    // The map should emit a signal on style loaded
    bool emitted = false;
    test.view.setMapChangeCallback([&](MapChange change) {
        if (change == mbgl::MapChangeDidFinishLoadingStyle) {
            emitted = true;
        }
    });
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    EXPECT_TRUE(emitted);
    
    // But not when the style couldn't be parsed
    emitted = false;
    map.setStyleJSON("invalid");
    EXPECT_FALSE(emitted);
}

TEST(Map, AddLayer) {
    MapTest test;

    Map map(test.view, test.fileSource, test.threadPool, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));

    auto layer = std::make_unique<BackgroundLayer>("background");
    layer->setBackgroundColor({{ 1, 0, 0, 1 }});
    map.addLayer(std::move(layer));

    test::checkImage("test/fixtures/map/add_layer", test::render(map));
}

TEST(Map, RemoveLayer) {
    MapTest test;

    Map map(test.view, test.fileSource, test.threadPool, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));

    auto layer = std::make_unique<BackgroundLayer>("background");
    layer->setBackgroundColor({{ 1, 0, 0, 1 }});
    map.addLayer(std::move(layer));
    map.removeLayer("background");

    test::checkImage("test/fixtures/map/remove_layer", test::render(map));
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

    Map map(test.view, test.fileSource, test.threadPool, MapMode::Still);
    map.setZoom(1);

    // This stylesheet has two raster layers, one that starts at zoom 1, the other at zoom 0.
    // We first render a map at zoom level 1, which should show both layers (both are "visible" due
    // to an opacity of 0.5). Then, we are zooming back out to a zoom level of 0.5 and rerender.
    // The "raster1" layer should not be visible anymore since it has minzoom 1, while "raster2"
    // should still be there. Both layers have a distinct color through "raster-hue-rotate".
    map.setStyleJSON(R"STYLE(
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

    test::checkImage("test/fixtures/map/disabled_layers/first", test::render(map));
    map.setZoom(0.5);
    test::checkImage("test/fixtures/map/disabled_layers/second", test::render(map));
}

TEST(Map, Classes) {
    MapTest test;

    Map map(test.view, test.fileSource, test.threadPool, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));

    EXPECT_FALSE(map.getTransitionOptions().duration);

    auto duration = mbgl::Duration(mbgl::Milliseconds(300));
    map.setTransitionOptions({ duration });
    EXPECT_EQ(map.getTransitionOptions().duration, duration);

    map.addClass("test");
    EXPECT_TRUE(map.hasClass("test"));

    map.removeClass("test");
    EXPECT_TRUE(map.getClasses().empty());

    std::vector<std::string> classes = { "foo", "bar" };
    map.setClasses(classes);
    EXPECT_FALSE(map.hasClass("test"));
    EXPECT_TRUE(map.hasClass("foo"));
    EXPECT_TRUE(map.hasClass("bar"));

    // Does nothing - same style JSON.
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    EXPECT_TRUE(map.hasClass("foo"));
    EXPECT_EQ(map.getTransitionOptions().duration, duration);

    map.setStyleJSON(util::read_file("test/fixtures/api/water.json"));
    EXPECT_TRUE(map.getClasses().empty());
    EXPECT_FALSE(map.getTransitionOptions().duration);
}

TEST(Map, AddImage) {
    MapTest test;

    Map map(test.view, test.fileSource, test.threadPool, MapMode::Still);
    auto decoded1 = decodeImage(util::read_file("test/fixtures/sprites/default_marker.png"));
    auto decoded2 = decodeImage(util::read_file("test/fixtures/sprites/default_marker.png"));
    auto image1 = std::make_unique<SpriteImage>(std::move(decoded1), 1.0);
    auto image2 = std::make_unique<SpriteImage>(std::move(decoded2), 1.0);

    // No-op.
    map.addImage("test-icon", std::move(image1));

    map.setStyleJSON(util::read_file("test/fixtures/api/icon_style.json"));
    map.addImage("test-icon", std::move(image2));
    test::checkImage("test/fixtures/map/add_icon", test::render(map));
}

TEST(Map, RemoveImage) {
    MapTest test;

    Map map(test.view, test.fileSource, test.threadPool, MapMode::Still);
    auto decoded = decodeImage(util::read_file("test/fixtures/sprites/default_marker.png"));
    auto image = std::make_unique<SpriteImage>(std::move(decoded), 1.0);

    map.setStyleJSON(util::read_file("test/fixtures/api/icon_style.json"));
    map.addImage("test-icon", std::move(image));
    map.removeImage("test-icon");
    test::checkImage("test/fixtures/map/remove_icon", test::render(map));
}

