#include "storage.hpp"

#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/storage/sqlite_cache.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <mbgl/platform/log.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/util/io.hpp>

using namespace mbgl;
using namespace std::literals::chrono_literals;
using namespace std::literals::string_literals;

namespace {

void checkRendering(Map& map, const char *name, std::chrono::milliseconds timeout) {
    test::checkImage("test/fixtures/stale/"s + name, test::render(map, timeout), 0.001, 0.1);
}

Response expiredItem(const std::string& path) {
    Response response;
    response.data = std::make_shared<std::string>(util::read_file("test/fixtures/"s + path));
    response.expires = 0s;
    return response;
}

const std::string prefix = "http://127.0.0.1:3000";

}

auto display = std::make_shared<mbgl::HeadlessDisplay>();

TEST_F(Storage, CacheStaleStyle) {
    HeadlessView view(display, 1);

    auto cache = SQLiteCache::getShared(":memory:");

    // Rig the cache with an expired stylesheet.
    const std::string stylePath = "stale/style.json";
    const Resource styleResource{ Resource::Kind::Style, prefix + "/" + stylePath };
    cache->put(styleResource, expiredItem(stylePath));

    DefaultFileSource fileSource(":memory:", ".");

    Map map(view, fileSource, MapMode::Still);
    map.setStyleURL(styleResource.url);

    checkRendering(map, "stale_style", 1000ms);
}

TEST_F(Storage, CacheStaleStyleAndTileJSON) {
    HeadlessView view(display, 1);

    auto cache = SQLiteCache::getShared(":memory:");

    // Rig the cache with an expired stylesheet.
    const std::string stylePath = "stale/style_and_tilejson.json";
    const Resource styleResource{ Resource::Kind::Style, prefix + "/" + stylePath };
    cache->put(styleResource, expiredItem(stylePath));

    // Rig the cache with an expired TileJSON.
    const std::string tilejsonPath = "stale/streets.json";
    const Resource tilejsonResource{ Resource::Kind::Source, prefix + "/" + tilejsonPath };
    cache->put(tilejsonResource, expiredItem(tilejsonPath));

    DefaultFileSource fileSource(":memory:", ".");

    Map map(view, fileSource, MapMode::Still);
    map.setStyleURL(styleResource.url);

    checkRendering(map, "stale_style_and_tilejson", 1000ms);
}
