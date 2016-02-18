#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>

#include <future>

TEST(API, RepeatedRender) {
    using namespace mbgl;

    const auto style = util::read_file("test/fixtures/api/water.json");

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

    {
        map.setStyleJSON(style, "");
        std::promise<PremultipliedImage> promise;
        map.renderStill([&promise](std::exception_ptr, PremultipliedImage&& image) {
            promise.set_value(std::move(image));
        });
        auto result = promise.get_future().get();
        ASSERT_EQ(256, result.width);
        ASSERT_EQ(512, result.height);
#if !TEST_READ_ONLY
        util::write_file("test/fixtures/api/1.png", encodePNG(result));
#endif
    }

    {
        map.setStyleJSON(style, "");
        std::promise<PremultipliedImage> promise;
        map.renderStill([&promise](std::exception_ptr, PremultipliedImage&& image) {
            promise.set_value(std::move(image));
        });
        auto result = promise.get_future().get();
        ASSERT_EQ(256, result.width);
        ASSERT_EQ(512, result.height);
#if !TEST_READ_ONLY
        util::write_file("test/fixtures/api/2.png", encodePNG(result));
#endif
    }

    auto observer = Log::removeObserver();
    auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    auto unchecked = flo->unchecked();
    EXPECT_TRUE(unchecked.empty()) << unchecked;
}
