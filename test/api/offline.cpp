#include "../fixtures/util.hpp"

#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <mbgl/platform/log.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/util/io.hpp>

using namespace mbgl;
using namespace std::literals::chrono_literals;
using namespace std::literals::string_literals;

namespace {

Response expiredItem(const std::string& path) {
    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/"s + path));
    response.expires = SystemClock::from_time_t(0);
    return response;
}

const std::string prefix = "http://127.0.0.1:3000";

}

auto display = std::make_shared<mbgl::HeadlessDisplay>();

TEST(API, Offline) {
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(":memory:", ".");

    fileSource.put(Resource::style(prefix + "/offline/style.json"), expiredItem("offline/style.json"));
    fileSource.put(Resource::source(prefix + "/offline/streets.json"), expiredItem("offline/streets.json"));
    fileSource.put(Resource::spriteJSON(prefix + "/offline/sprite", 1.0), expiredItem("offline/sprite.json"));
    fileSource.put(Resource::spriteImage(prefix + "/offline/sprite", 1.0), expiredItem("offline/sprite.png"));
    fileSource.put(Resource::tile(prefix + "/offline/{z}-{x}-{y}.vector.pbf", 1.0, 0, 0, 0), expiredItem("offline/0-0-0.vector.pbf"));
    fileSource.put(Resource::glyphs(prefix + "/offline/{fontstack}/{range}.pbf", "Helvetica", {0, 255}), expiredItem("offline/glyph.pbf"));
    NetworkStatus::Set(NetworkStatus::Status::Offline);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleURL(prefix + "/offline/style.json");

    test::checkImage("test/fixtures/offline"s,
                     test::render(map),
                     0.0015,
                     0.1);

    NetworkStatus::Set(NetworkStatus::Status::Online);
}
