#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/map/backend_scope.hpp>
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
    BackendScope scope { backend };
    OffscreenView view { backend.getContext(), { 128, 512 } };
    StubFileSource fileSource;
    ThreadPool threadPool(4);

    std::unique_ptr<Map> map =
        std::make_unique<Map>(backend, MapObserver::nullObserver(), view.getSize(), 1, fileSource, threadPool, MapMode::Still);
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
