#include "../fixtures/util.hpp"

#include <mbgl/annotation/point_annotation.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/still_image.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>

#include <future>
#include <vector>

TEST(Annotations, PointAnnotation) {
    using namespace mbgl;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.addPointAnnotation(PointAnnotation({ 0, 0 }, "default_marker"));

    std::promise<std::unique_ptr<const StillImage>> promise;
    map.renderStill([&promise](std::exception_ptr, std::unique_ptr<const StillImage> image) {
        promise.set_value(std::move(image));
    });

    auto result = promise.get_future().get();
    const std::string png = util::compress_png(result->width, result->height, result->pixels.get());
    util::write_file("test/output/point_annotation.png", png);
}
