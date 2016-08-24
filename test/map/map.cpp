#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/fake_file_source.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/storage/default_file_source.hpp>
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
};

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

    Map map(test.view, fileSource, MapMode::Still);
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
        Map map(test.view, test.fileSource, MapMode::Still);
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

    Map map(test.view, test.fileSource, MapMode::Still);
    map.setStyleURL("mapbox://bar");

    test.runLoop.run();
}

TEST(Map, DoubleStyleLoad) {
    MapTest test;

    Map map(test.view, test.fileSource, MapMode::Still);
    map.setStyleJSON("");
    map.setStyleJSON("");
}

TEST(Map, StyleFresh) {
    // The map should not revalidate fresh styles.

    MapTest test;
    FakeFileSource fileSource;

    Map map(test.view, fileSource, MapMode::Still);
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

    Map map(test.view, fileSource, MapMode::Still);
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

    Map map(test.view, fileSource, MapMode::Still);
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

    Map map(test.view, fileSource, MapMode::Still);
    map.setStyleURL("mapbox://styles/test");
    map.addLayer(std::make_unique<style::BackgroundLayer>("bg"));

    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/api/water.json"));
    fileSource.respond(Resource::Style, response);

    EXPECT_EQ(0u, fileSource.requests.size());
    EXPECT_NE(nullptr, map.getLayer("water"));
}

TEST(Map, AddLayer) {
    MapTest test;

    Map map(test.view, test.fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));

    auto layer = std::make_unique<BackgroundLayer>("background");
    layer->setBackgroundColor({{ 1, 0, 0, 1 }});
    map.addLayer(std::move(layer));

    test::checkImage("test/fixtures/map/add_layer", test::render(map));
}

TEST(Map, RemoveLayer) {
    MapTest test;

    Map map(test.view, test.fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));

    auto layer = std::make_unique<BackgroundLayer>("background");
    layer->setBackgroundColor({{ 1, 0, 0, 1 }});
    map.addLayer(std::move(layer));
    map.removeLayer("background");

    test::checkImage("test/fixtures/map/remove_layer", test::render(map));
}
