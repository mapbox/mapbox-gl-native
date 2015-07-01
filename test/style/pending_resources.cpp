#include "../fixtures/fixture_log_observer.hpp"
#include "../fixtures/util.hpp"
#include "mock_file_source.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/map/still_image.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/uv_detail.hpp>

using namespace mbgl;

class PendingResources : public ::testing::TestWithParam<std::string> {
};

// This test will load a Style but one of the resources requested will not be
// replied immediately like the others. We get an notification by the
// MockFileSource when some resource is artificially delayed and we destroy
// the Map object after that. The idea here is to test if these pending requests
// are getting canceled correctly if on shutdown.
TEST_P(PendingResources, DeleteMapObjectWithPendingRequest) {
    // TODO: The glyphs test is blocked by the issue #1664.
    if (GetParam() == "glyphs.pbf") {
        return;
    }

    util::RunLoop loop(uv_default_loop());

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1, 1000, 1000);
    MockFileSource fileSource(MockFileSource::SuccessWithDelay, GetParam());

    std::unique_ptr<Map> map = std::make_unique<Map>(view, fileSource, MapMode::Still);

    uv::async endTest(loop.get(), [&map, &loop] {
        map.reset();
        loop.stop();
    });

    endTest.unref();
    fileSource.setOnRequestDelayedCallback([&endTest] { endTest.send(); });

    const std::string style = util::read_file("test/fixtures/resources/style.json");
    map->setStyleJSON(style, ".");

    map->renderStill([&endTest](std::exception_ptr, std::unique_ptr<const StillImage>) {
        EXPECT_TRUE(false) << "Should never happen.";
    });

    uv_run(loop.get(), UV_RUN_DEFAULT);
}

// In the test data below, "sprite" will match both "sprite.json" and "sprite.png" and cause two
// requests to be canceled. "resources" will match everything but in practice will only test the
// cancellation of the sprites and "source_*.json" because we only load the rest after "source_*.json"
// gets parsed.
INSTANTIATE_TEST_CASE_P(Style, PendingResources,
    ::testing::Values(
        "source_raster.json",
        "source_vector.json",
        "sprite.json",
        "sprite.png",
        "sprite",
        "raster.png",
        "vector.pbf",
        "glyphs.pbf",
        "resources"));
