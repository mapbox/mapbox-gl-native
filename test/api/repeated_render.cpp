#include "../fixtures/util.hpp"
#include "../fixtures/fixture_log_observer.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/map/still_image.hpp>
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
    DefaultFileSource fileSource(nullptr);

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    Map map(view, fileSource, MapMode::Still);

    {
        map.setStyleJSON(style, "");
        std::promise<std::unique_ptr<const StillImage>> promise;
        map.renderStill([&promise](std::exception_ptr, std::unique_ptr<const StillImage> image) {
            promise.set_value(std::move(image));
        });
        auto result = promise.get_future().get();
        ASSERT_EQ(256, result->width);
        ASSERT_EQ(512, result->height);
        const std::string png = util::compress_png(result->width, result->height, result->pixels.get());
        util::write_file("test/fixtures/api/1.png", png);
    }

    {
        map.setStyleJSON(style, "TEST_DATA/suite");
        std::promise<std::unique_ptr<const StillImage>> promise;
        map.renderStill([&promise](std::exception_ptr, std::unique_ptr<const StillImage> image) {
            promise.set_value(std::move(image));
        });
        auto result = promise.get_future().get();
        ASSERT_EQ(256, result->width);
        ASSERT_EQ(512, result->height);
        const std::string png = util::compress_png(result->width, result->height, result->pixels.get());
        util::write_file("test/fixtures/api/2.png", png);
    }

    auto observer = Log::removeObserver();
    auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    auto unchecked = flo->unchecked();
    EXPECT_TRUE(unchecked.empty()) << unchecked;
}
