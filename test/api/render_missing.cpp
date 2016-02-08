#include "../fixtures/util.hpp"
#include "../fixtures/fixture_log_observer.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>

#include <future>

TEST(API, RenderMissingTile) {
    using namespace mbgl;

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
    map.setStyleJSON(style, "");
    std::promise<void> promise;
    map.renderStill([&promise, &message](std::exception_ptr err, PremultipliedImage&&) {
        ASSERT_TRUE(err.operator bool());
        try {
            std::rethrow_exception(err);
        } catch (const std::exception& ex) {
            message = ex.what();
#ifdef MBGL_HTTP_NSURL
            EXPECT_STREQ("Could not connect to the server.", ex.what());
#elif MBGL_HTTP_CURL
            const char* prefix = "Couldn't connect to server:";
            EXPECT_EQ(0, strncmp(prefix, ex.what(), strlen(prefix))) << "Full message is: \""
                                                                     << ex.what() << "\"";
#else
            FAIL();
#endif
        }
        promise.set_value();
    });
    promise.get_future().get();

    auto observer = Log::removeObserver();
    auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    EXPECT_EQ(1, flo->count(FixtureLog::Message(
                     EventSeverity::Error, Event::Style, -1,
                     std::string("Failed to load tile 0/0/0 for source mapbox: " + message))));
    auto unchecked = flo->unchecked();
    EXPECT_TRUE(unchecked.empty()) << unchecked;
}
