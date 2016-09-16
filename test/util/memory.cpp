#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/util.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <utility>

#include <stdlib.h>
#include <unistd.h>

using namespace mbgl;
using namespace std::literals::string_literals;

long getRSS() {
    auto statm = util::read_file("/proc/self/statm");

    std::vector<std::string> stats;
    std::istringstream stream(statm);

    std::copy(std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>(),
              std::back_inserter(stats));

    return std::stol(stats[1]) * getpagesize();
}

bool isUsingJemalloc() {
    const char* preload = getenv("LD_PRELOAD");

    if (preload) {
        return std::string(preload).find("libjemalloc.so") != std::string::npos;
    } else {
        return false;
    }
}

class MemoryTest {
public:
    MemoryTest() {
        fileSource.styleResponse = [&](const Resource& res) {
            return response("style_" + getType(res) + ".json");
        };
        fileSource.tileResponse = [&](const Resource& res) {
            return response(getType(res) + ".tile");
        };
        fileSource.sourceResponse = [&](const Resource& res) {
            return response("source_" + getType(res) + ".json");
        };
        fileSource.glyphsResponse = [&](const Resource&) { return response("glyphs.pbf"); };
        fileSource.spriteJSONResponse = [&](const Resource&) { return response("sprite.json"); };
        fileSource.spriteImageResponse = [&](const Resource&) { return response("sprite.png"); };
    }

    util::RunLoop runLoop;
    std::shared_ptr<HeadlessDisplay> display{std::make_shared<mbgl::HeadlessDisplay>()};
    HeadlessView view{display, 2};
    StubFileSource fileSource;

private:
    Response response(const std::string& path) {
        Response result;

        auto it = cache.find(path);
        if (it != cache.end()) {
            result.data = it->second;
        } else {
            auto data =
                std::make_shared<std::string>(util::read_file("test/fixtures/resources/"s + path));

            cache.insert(it, std::make_pair(path, data));
            result.data = data;
        }

        return result;
    }

    std::string getType(const Resource& res) {
        if (res.url.find("satellite") != std::string::npos) {
            return "raster";
        } else {
            return "vector";
        }
    };

    std::unordered_map<std::string, std::shared_ptr<std::string>> cache;
};

TEST(Memory, Vector) {
    MemoryTest test;

    Map map(test.view, test.fileSource, MapMode::Still);
    map.setZoom(16); // more map features
    map.setStyleURL("mapbox://streets");

    test::render(map);
}

TEST(Memory, Raster) {
    MemoryTest test;

    Map map(test.view, test.fileSource, MapMode::Still);
    map.setStyleURL("mapbox://satellite");

    test::render(map);
}

// This test will render 3 map objects alternating
// between a raster and a vector style. Memory is
// expected to grow between the renderings due to
// fragmentation. A good allocator for Mapbox GL
// Native will keep memory growth within acceptable
// levels and stable in the long run.
TEST(Memory, Fragmentation) {
    if (!isUsingJemalloc()) {
        return;
    }

    MemoryTest test;

    Map map1(test.view, test.fileSource, MapMode::Still);
    Map map2(test.view, test.fileSource, MapMode::Still);
    Map map3(test.view, test.fileSource, MapMode::Still);

    map1.setZoom(16);
    map2.setZoom(16);
    map3.setZoom(16);

    auto renderMap = [&] {
        map1.setStyleURL("mapbox://satellite");
        test::render(map1);

        map2.setStyleURL("mapbox://satellite");
        test::render(map2);

        map3.setStyleURL("mapbox://satellite");
        test::render(map3);

        map1.setStyleURL("mapbox://streets");
        test::render(map1);

        map2.setStyleURL("mapbox://streets");
        test::render(map2);

        map3.setStyleURL("mapbox://streets");
        test::render(map3);
    };

    // Warm up buffers and cache.
    for (unsigned i = 0; i < 5; ++i) {
        renderMap();
    }

    long lastRSS = getRSS();
    long memoryFragments = 0;

    for (unsigned i = 0; i < 20; ++i) {
        renderMap();

        long currentRSS = getRSS();

        memoryFragments += currentRSS - lastRSS;
        lastRSS = currentRSS;
    }

    ASSERT_LT(memoryFragments, 10 * 1024 * 1024) << "\
        Abnormal memory growth detected.";
}

// This test will measure the size of a Map object
// after rendering a raster and a vector style. The
// idea is to try to keep the memory footprint within
// reasonable limits, so this test acts more like a
// safeguard.
TEST(Memory, Footprint) {
    if (!isUsingJemalloc()) {
        return;
    }

    MemoryTest test;

    auto renderMap = [](Map* map, const char* style) {
        map->setZoom(16);

        map->setStyleURL(style);
        test::render(*map);
    };

    // Warm up buffers and cache.
    for (unsigned i = 0; i < 10; ++i) {
        Map map(test.view, test.fileSource, MapMode::Still);
        renderMap(&map, "mapbox://streets");
        renderMap(&map, "mapbox://satellite");
    };

    // Process close callbacks, mostly needed by
    // libuv runloop.
    test.runLoop.runOnce();

    std::vector<std::unique_ptr<Map>> maps;
    unsigned runs = 15;

    long vectorInitialRSS = getRSS();
    for (unsigned i = 0; i < runs; ++i) {
        auto vector = std::make_unique<Map>(test.view, test.fileSource, MapMode::Still);
        renderMap(vector.get(), "mapbox://streets");
        maps.push_back(std::move(vector));
    };

    double vectorFootprint = (getRSS() - vectorInitialRSS) / double(runs);

    long rasterInitialRSS = getRSS();
    for (unsigned i = 0; i < runs; ++i) {
        auto raster = std::make_unique<Map>(test.view, test.fileSource, MapMode::Still);
        renderMap(raster.get(), "mapbox://satellite");
        maps.push_back(std::move(raster));
    };

    double rasterFootprint = (getRSS() - rasterInitialRSS) / double(runs);

    ASSERT_LT(vectorFootprint, 65 * 1024 * 1024) << "\
        mbgl::Map footprint over 65MB for vector styles.";

    ASSERT_LT(rasterFootprint, 25 * 1024 * 1024) << "\
        mbgl::Map footprint over 25MB for raster styles.";
}
