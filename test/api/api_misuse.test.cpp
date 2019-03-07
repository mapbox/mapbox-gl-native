#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/platform/factory.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/gl/headless_frontend.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/run_loop.hpp>

#include <future>

using namespace mbgl;


TEST(API, RenderWithoutCallback) {
    auto log = new FixtureLogObserver();
    Log::setObserver(std::unique_ptr<Log::Observer>(log));

    util::RunLoop loop;

    FileSourceOptions stubFileSourceOptions;
    auto fileSource = platform::Factory::sharedFileSource(stubFileSourceOptions, std::make_shared<StubFileSource>());

    float pixelRatio { 1 };

    HeadlessFrontend frontend { pixelRatio, stubFileSourceOptions };

    auto map = std::make_unique<Map>(frontend, MapObserver::nullObserver(), frontend.getSize(),
                                     pixelRatio, MapOptions().withMapMode(MapMode::Static), stubFileSourceOptions);
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
