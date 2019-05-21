#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/fixture_log_observer.hpp>
#include <mbgl/test/map_adapter.hpp>

#include <mbgl/map/map_options.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/run_loop.hpp>

#include <future>

using namespace mbgl;


TEST(API, RenderWithoutCallback) {
    auto log = new FixtureLogObserver();
    Log::setObserver(std::unique_ptr<Log::Observer>(log));

    util::RunLoop loop;

    HeadlessFrontend frontend { 1 };

    auto map = std::make_unique<MapAdapter>(frontend, MapObserver::nullObserver(),
                                            std::make_shared<StubFileSource>(),
                                            MapOptions().withMapMode(MapMode::Static).withSize(frontend.getSize()));
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
