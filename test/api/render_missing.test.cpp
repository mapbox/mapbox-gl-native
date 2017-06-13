#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/map/backend_scope.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/offscreen_view.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/style/style.hpp>

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

    HeadlessBackend backend { test::sharedDisplay() };
    BackendScope scope { backend };
    OffscreenView view { backend.getContext(), { 256, 512 } };
    DefaultFileSource fileSource(":memory:", "test/fixtures/api/assets");
    ThreadPool threadPool(4);

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    Map map(backend, MapObserver::nullObserver(), view.getSize(), 1, fileSource, threadPool, MapMode::Still);

    std::string message;

    // This host does not respond (== connection error).
    // Are you seeing this test fail? Make sure you don't have a server running on port 3001!
    map.getStyle().loadJSON(style);
    map.renderStill(view, [&](std::exception_ptr err) {
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
