#include "../fixtures/util.hpp"
#include "../fixtures/fixture_log_observer.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/map/still_image.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/exception.hpp>

#include <future>

using namespace mbgl;

TEST(API, RenderWithoutCallback) {
    FixtureLogObserver* log = new FixtureLogObserver();
    Log::setObserver(std::unique_ptr<Log::Observer>(log));

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    view.resize(128, 512);
    DefaultFileSource fileSource(nullptr);

    std::unique_ptr<Map> map = std::make_unique<Map>(view, fileSource, MapMode::Still);
    map->renderStill(nullptr);

    // Force Map thread to join.
    map.reset();

    const FixtureLogObserver::LogMessage logMessage {
        EventSeverity::Error,
        Event::General,
        int64_t(-1),
        "StillImageCallback not set",
    };

    EXPECT_EQ(log->count(logMessage), 1u);
}

TEST(API, RenderWithoutStyle) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    view.resize(128, 512);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);

    std::promise<std::exception_ptr> promise;
    map.renderStill([&promise](std::exception_ptr error, std::unique_ptr<const StillImage>) {
        promise.set_value(error);
    });

    try {
        std::rethrow_exception(promise.get_future().get());
    } catch (const util::MisuseException& ex) {
        EXPECT_EQ(std::string(ex.what()), "Map doesn't have a style");
    } catch (const std::exception&) {
        EXPECT_TRUE(false) << "Unhandled exception.";
    }
}
