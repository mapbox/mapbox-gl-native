#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/getrss.hpp>
#include <mbgl/test/util.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/gl/headless_frontend.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/style/style.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <utility>

#include <cstdlib>
#include <unistd.h>

using namespace mbgl;
using namespace std::literals::string_literals;

class MemoryTest {
public:
    MemoryTest() {
        fileSource.styleResponse = [&](const Resource& res) { return response("style_" + getType(res) + ".json");};
        fileSource.tileResponse = [&](const Resource& res) { return response(getType(res) + ".tile"); };
        fileSource.sourceResponse = [&](const Resource& res) { return response("source_" + getType(res) + ".json"); };
        fileSource.glyphsResponse = [&](const Resource&) { return response("glyphs.pbf"); };
        fileSource.spriteJSONResponse = [&](const Resource&) { return response("sprite.json"); };
        fileSource.spriteImageResponse = [&](const Resource&) { return response("sprite.png"); };
    }

    util::RunLoop runLoop;
    StubFileSource fileSource;
    ThreadPool threadPool { 4 };

private:
    Response response(const std::string& path) {
        Response result;

        auto it = cache.find(path);
        if (it != cache.end()) {
            result.data = it->second;
        } else {
            auto data = std::make_shared<std::string>(
                util::read_file("test/fixtures/resources/"s + path));

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
    float ratio { 2 };

    HeadlessFrontend frontend { { 256, 256 }, ratio, test.fileSource, test.threadPool };
    Map map(frontend, MapObserver::nullObserver(), frontend.getSize(), ratio, test.fileSource,
            test.threadPool, MapMode::Static);
    map.setZoom(16); // more map features
    map.getStyle().loadURL("mapbox://streets");

    frontend.render(map);
}

TEST(Memory, Raster) {
    MemoryTest test;
    float ratio { 2 };

    HeadlessFrontend frontend { { 256, 256 }, ratio, test.fileSource, test.threadPool };
    Map map(frontend, MapObserver::nullObserver(), frontend.getSize(), ratio, test.fileSource,
            test.threadPool, MapMode::Static);
    map.getStyle().loadURL("mapbox://satellite");

    frontend.render(map);
}

/**
On CI, we only run the memory footprint test in the Qt build, because it uses
jemalloc, which yields more consistent memory usage results.  To force it to
run locally, use `DO_MEMORY_FOOTPRINT=1 make run-test-Memory.Footprint.
*/
bool shouldRunFootprint() {
    const char* preload = getenv("LD_PRELOAD");
    
    if (preload) {
        return std::string(preload).find("libjemalloc.so") != std::string::npos;
    } else {
        return getenv("DO_MEMORY_FOOTPRINT");
    }
}

// This test will measure the size of a Map object
// after rendering a raster and a vector style. The
// idea is to try to keep the memory footprint within
// reasonable limits, so this test acts more like a
// safeguard.
TEST(Memory, Footprint) {
    if (!shouldRunFootprint()) {
        return;
    }

    MemoryTest test;

    class FrontendAndMap {
    public:
        FrontendAndMap(MemoryTest& test_, const char* style)
            : frontend(Size{ 256, 256 }, 2, test_.fileSource, test_.threadPool)
            , map(frontend, MapObserver::nullObserver(), frontend.getSize(), 2, test_.fileSource, test_.threadPool, MapMode::Static) {
            map.setZoom(16);
            map.getStyle().loadURL(style);
            frontend.render(map);
        }

        HeadlessFrontend frontend;
        Map map;
    };

    // Warm up buffers and cache.
    for (unsigned i = 0; i < 10; ++i) {
        FrontendAndMap(test, "mapbox://streets");
        FrontendAndMap(test, "mapbox://satellite");
    }

    // Process close callbacks, mostly needed by
    // libuv runloop.
    test.runLoop.runOnce();

    std::vector<std::unique_ptr<FrontendAndMap>> maps;
    unsigned runs = 15;

    long vectorInitialRSS = mbgl::test::getCurrentRSS();
    for (unsigned i = 0; i < runs; ++i) {
        maps.emplace_back(std::make_unique<FrontendAndMap>(test, "mapbox://streets"));
    }

    double vectorFootprint = (mbgl::test::getCurrentRSS() - vectorInitialRSS) / double(runs);

    long rasterInitialRSS = mbgl::test::getCurrentRSS();
    for (unsigned i = 0; i < runs; ++i) {
        maps.emplace_back(std::make_unique<FrontendAndMap>(test, "mapbox://satellite"));
    }

    double rasterFootprint = (mbgl::test::getCurrentRSS() - rasterInitialRSS) / double(runs);
    
    RecordProperty("vectorFootprint", vectorFootprint);
    RecordProperty("rasterFootprint", rasterFootprint);

    ASSERT_LT(vectorFootprint, 40 * 1024 * 1024) << "\
        mbgl::Map footprint over 65.2MB for vector styles.";

    ASSERT_LT(rasterFootprint, 25 * 1024 * 1024) << "\
        mbgl::Map footprint over 25MB for raster styles.";
}
