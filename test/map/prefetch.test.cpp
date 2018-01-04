#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/gl/headless_frontend.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>

#include <algorithm>
#include <string>
#include <vector>

using namespace mbgl;
using namespace mbgl::style;
using namespace std::literals::string_literals;

TEST(Map, PrefetchTiles) {
    util::RunLoop runLoop;
    ThreadPool threadPool(4);
    StubFileSource fileSource;
    HeadlessFrontend frontend { { 512, 512 }, 1, fileSource, threadPool };
    Map map(frontend, MapObserver::nullObserver(), frontend.getSize(), 1, fileSource, threadPool, MapMode::Static);

    std::vector<int> tiles;

    fileSource.response = [&] (const Resource& res) -> optional<Response> {
        Response response;

        auto zoom = std::stoi(res.url);
        tiles.push_back(zoom);

        // Return a red tile for prefetched tiles or green to the actual tile.
        // The end rendering result should be all green because the map is only
        // considered fully rendered when only ideal tiles are shown.
        if (zoom == int(map.getZoom()) + 1) {
            response.data = std::make_shared<std::string>(
                util::read_file("test/fixtures/map/prefetch/tile_green.png"));
        } else {
            response.data = std::make_shared<std::string>(
                util::read_file("test/fixtures/map/prefetch/tile_red.png"));
        }

        return { std::move(response) };
    };

    auto checkTilesForZoom = [&](int zoom, const std::vector<int>& expected) {
        tiles.clear();

        // Force tile reloading.
        map.getStyle().loadJSON(util::read_file("test/fixtures/map/prefetch/empty.json"));
        map.getStyle().loadJSON(util::read_file("test/fixtures/map/prefetch/style.json"));

        map.setLatLngZoom({ 40.726989, -73.992857 }, zoom); // Manhattan

        // Should always render the ideal tiles (i.e. a green map)
        test::checkImage("test/fixtures/map/prefetch", frontend.render(map));

        ASSERT_TRUE(std::is_permutation(tiles.begin(), tiles.end(), expected.begin()));
        ASSERT_FALSE(tiles.empty());
    };

    // Check defaults, should be 4.
    ASSERT_EQ(map.getPrefetchZoomDelta(), 4);
    checkTilesForZoom(12, { 13, 13, 13, 13, 13, 13, 13, 13, 13, 9 });

    // Setting it to 0 disables prefetching.
    map.setPrefetchZoomDelta(0);

    // No prefetching, raster tiles will use ideal
    // tiles instead of the actual zoom level, that is
    // why the zoom levels for non-prefetched tiles are
    // not the same.
    checkTilesForZoom(10, { 11, 11, 11, 11, 11, 11, 11, 11, 11 });

    map.setPrefetchZoomDelta(5);
    checkTilesForZoom(12, { 13, 13, 13, 13, 13, 13, 13, 13, 13, 8 });

    // Should clamp at `minzoom`.
    map.setPrefetchZoomDelta(20);
    checkTilesForZoom(10, { 11, 11, 11, 11, 11, 11, 11, 11, 11, 0 });

    // Disabled again.
    map.setPrefetchZoomDelta(0);
    checkTilesForZoom(13, { 14, 14, 14, 14, 14, 14, 14, 14, 14 });
}
