#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>

#include <future>

#if TEST_HAS_SERVER
#define TEST_REQUIRES_SERVER(name) name
#else
#define TEST_REQUIRES_SERVER(name) DISABLED_ ## name
#endif

TEST(API, TEST_REQUIRES_SERVER(RenderMissingTile)) {
    using namespace mbgl;

    util::RunLoop loop;

    const auto style = util::read_file("test/fixtures/api/water_missing_tiles.json");

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1, 256, 512);
#ifdef MBGL_ASSET_ZIP
    // Regenerate with `cd test/fixtures/api/ && zip -r assets.zip assets/`
    DefaultFileSource fileSource(":memory:", "test/fixtures/api/assets.zip");
#else
    DefaultFileSource fileSource(":memory:", "test/fixtures/api/assets");
#endif

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    Map map(view, fileSource, MapMode::Still);

    std::string message;

    // This host does not respond (== connection error).
    // Are you seeing this test fail? Make sure you don't have a server running on port 3001!
    map.setStyleJSON(style);
    map.renderStill([&](std::exception_ptr err, PremultipliedImage&&) {
        ASSERT_TRUE(err.operator bool());
        try {
            std::rethrow_exception(err);
        } catch (const std::exception& ex) {
            message = ex.what();
            EXPECT_TRUE(message.find("onnect") != std::string::npos); // [C|c]onnect
        }
        loop.stop();
    });

    loop.run();

    auto observer = Log::removeObserver();
    auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    EXPECT_EQ(1u, flo->count(FixtureLog::Message(
                     EventSeverity::Error, Event::Style, -1,
                     std::string("Failed to load tile 0/0/0=>0 for source mapbox: " + message))));
    auto unchecked = flo->unchecked();
    EXPECT_TRUE(unchecked.empty()) << unchecked;
}
