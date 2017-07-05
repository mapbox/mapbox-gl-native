#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/offscreen_view.hpp>
#include <mbgl/test/stub_renderer_frontend.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/run_loop.hpp>

#include <future>

using namespace mbgl;


TEST(API, RenderWithoutCallback) {
    auto log = new FixtureLogObserver();
    Log::setObserver(std::unique_ptr<Log::Observer>(log));

    util::RunLoop loop;

    HeadlessBackend backend;
    BackendScope scope { backend };
    OffscreenView view { backend.getContext(), { 128, 512 } };
    StubFileSource fileSource;
    ThreadPool threadPool(4);
    float pixelRatio { 1 };
    StubRendererFrontend rendererFrontend {
            std::make_unique<Renderer>(backend, pixelRatio, fileSource, threadPool), view };

    auto map = std::make_unique<Map>(rendererFrontend, MapObserver::nullObserver(), view.getSize(),
                                     pixelRatio, fileSource, threadPool, MapMode::Still);
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
