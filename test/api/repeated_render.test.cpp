#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_backend.hpp>
#include <mbgl/platform/default/offscreen_view.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>

#include <future>

using namespace mbgl;


TEST(API, RepeatedRender) {

    util::RunLoop loop;

    const auto style = util::read_file("test/fixtures/api/water.json");

    HeadlessBackend backend { test::sharedDisplay() };
    OffscreenView view { backend.getContext(), { 256, 512 } };
#ifdef MBGL_ASSET_ZIP
    // Regenerate with `cd test/fixtures/api/ && zip -r assets.zip assets/`
    DefaultFileSource fileSource(":memory:", "test/fixtures/api/assets.zip");
#else
    DefaultFileSource fileSource(":memory:", "test/fixtures/api/assets");
#endif

    ThreadPool threadPool(4);

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    Map map(backend, view.size, 1, fileSource, threadPool, MapMode::Still);

    {
        map.setStyleJSON(style);
        PremultipliedImage result;
        map.renderStill(view, [&](std::exception_ptr) {
            result = view.readStillImage();
        });

        while (!result.valid()) {
            loop.runOnce();
        }

        ASSERT_EQ(256u, result.size.width);
        ASSERT_EQ(512u, result.size.height);
#if !TEST_READ_ONLY
        util::write_file("test/fixtures/api/1.png", encodePNG(result));
#endif
    }

    {
        map.setStyleJSON(style);
        PremultipliedImage result;
        map.renderStill(view, [&](std::exception_ptr) {
            result = view.readStillImage();
        });

        while (!result.valid()) {
            loop.runOnce();
        }

        ASSERT_EQ(256u, result.size.width);
        ASSERT_EQ(512u, result.size.height);
#if !TEST_READ_ONLY
        util::write_file("test/fixtures/api/2.png", encodePNG(result));
#endif
    }

    auto observer = Log::removeObserver();
    auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    auto unchecked = flo->unchecked();
    EXPECT_TRUE(unchecked.empty()) << unchecked;
}
