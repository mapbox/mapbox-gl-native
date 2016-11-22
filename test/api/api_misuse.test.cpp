#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/offscreen_view.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/run_loop.hpp>

#include <future>

using namespace mbgl;


TEST(API, RenderWithoutCallback) {
    auto log = new FixtureLogObserver();
    Log::setObserver(std::unique_ptr<Log::Observer>(log));

    util::RunLoop loop;

    HeadlessBackend backend { test::sharedDisplay() };
    OffscreenView view { backend.getContext(), { 128, 512 } };
    StubFileSource fileSource;
    ThreadPool threadPool(4);

    std::unique_ptr<Map> map =
        std::make_unique<Map>(backend, view.size, 1, fileSource, threadPool, MapMode::Still);
    map->renderStill(view, nullptr);

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
    util::RunLoop loop;

    HeadlessBackend backend { test::sharedDisplay() };
    OffscreenView view { backend.getContext(), { 128, 512 } };
    StubFileSource fileSource;
    ThreadPool threadPool(4);

    Map map(backend, view.size, 1, fileSource, threadPool, MapMode::Still);

    std::exception_ptr error;
    map.renderStill(view, [&](std::exception_ptr error_) {
        error = error_;
        loop.stop();
    });

    loop.run();

    try {
        std::rethrow_exception(error);
    } catch (const util::MisuseException& ex) {
        EXPECT_EQ(std::string(ex.what()), "Map doesn't have a style");
    } catch (const std::exception&) {
        EXPECT_TRUE(false) << "Unhandled exception.";
    }
}
