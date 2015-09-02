#include "../fixtures/util.hpp"
#include "../fixtures/fixture_log_observer.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/map/still_image.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/annotation/sprite_image.hpp>

#include <mbgl/util/io.hpp>

#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <future>

using namespace mbgl;

TEST(Headless, CustomSpriteImages) {
    FixtureLog log;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    view.resize(256, 256);
    DefaultFileSource fileSource(nullptr);

    const auto style = util::read_file("test/fixtures/headless/pois.json");

    Map map(view, fileSource, MapMode::Still);

    map.setLatLngZoom(LatLng{ 52.499167, 13.418056 }, 15);

    map.setStyleJSON(style, "");
    map.setSprite("cafe",
                  std::make_shared<SpriteImage>(12, 12, 1, std::string(12 * 12 * 4, '\xFF')));
    std::promise<std::unique_ptr<const StillImage>> promise;
    map.renderStill([&promise](std::exception_ptr error, std::unique_ptr<const StillImage> image) {
        if (error) {
            promise.set_exception(error);
        } else {
            promise.set_value(std::move(image));
        }
    });
    auto result = promise.get_future().get();
    ASSERT_EQ(256, result->width);
    ASSERT_EQ(256, result->height);

    EXPECT_EQ(
        21u,
        log.count({
            EventSeverity::Info, Event::Sprite, int64_t(-1), "Can't find sprite named 'bakery'",
        }));

    // const size_t bytes = result->width * result->height * 4;
    // const auto hash = test::crc64(reinterpret_cast<const char*>(result->pixels.get()), bytes);
    // EXPECT_EQ(0xC40A4BCD76AEC564u, hash) << std::hex << hash;

    // const std::string png = util::compress_png(result->width, result->height,
    // result->pixels.get());
    // util::write_file("test/fixtures/headless/1.actual.png", png);
}
