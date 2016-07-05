#include <mbgl/test/util.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

#include <mapbox/pixelmatch.hpp>

#include <csignal>

#include <unistd.h>

namespace mbgl {
namespace test {

PremultipliedImage render(Map& map) {
    PremultipliedImage result;
    map.renderStill([&result](std::exception_ptr, PremultipliedImage&& image) {
        result = std::move(image);
    });

    while (!result.size()) {
        util::RunLoop::Get()->runOnce();
    }

    return result;
}

void checkImage(const std::string& base,
                const PremultipliedImage& actual,
                double imageThreshold,
                double pixelThreshold) {
#if !TEST_READ_ONLY
    if (getenv("UPDATE")) {
        util::write_file(base + "/expected.png", encodePNG(actual));
        return;
    }
#endif

    PremultipliedImage expected = decodeImage(util::read_file(base + "/expected.png"));
    PremultipliedImage diff { expected.width, expected.height };

    ASSERT_EQ(expected.width, actual.width);
    ASSERT_EQ(expected.height, actual.height);

    double pixels = mapbox::pixelmatch(actual.data.get(),
                                       expected.data.get(),
                                       expected.width,
                                       expected.height,
                                       diff.data.get(),
                                       pixelThreshold);

    EXPECT_LE(pixels / (expected.width * expected.height), imageThreshold);

#if !TEST_READ_ONLY
    util::write_file(base + "/actual.png", encodePNG(actual));
    util::write_file(base + "/diff.png", encodePNG(diff));
#endif
}

} // namespace test
} // namespace mbgl
