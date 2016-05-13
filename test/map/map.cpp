#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>

using namespace mbgl;
using namespace std::literals::string_literals;

TEST(Map, Offline) {
    util::RunLoop runLoop;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
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
    fileSource.put(Resource::tile(prefix + "{z}-{x}-{y}.vector.pbf", 1.0, 0, 0, 0), expiredItem("0-0-0.vector.pbf"));
    fileSource.put(Resource::glyphs(prefix + "{fontstack}/{range}.pbf", {{"Helvetica"}}, {0, 255}), expiredItem("glyph.pbf"));
    NetworkStatus::Set(NetworkStatus::Status::Offline);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleURL(prefix + "style.json");

    test::checkImage("test/fixtures/map/offline",
                     test::render(map),
                     0.0015,
                     0.1);

    NetworkStatus::Set(NetworkStatus::Status::Online);
}

TEST(Map, DoubleStyleLoad) {
    util::RunLoop runLoop;

    std::shared_ptr<HeadlessDisplay> display = std::make_shared<HeadlessDisplay>();
    HeadlessView view(display, 1, 512, 512);
    StubFileSource fileSource;

    Map map(view, fileSource);
    map.setStyleJSON("", "");
    map.setStyleJSON("", "");
}
