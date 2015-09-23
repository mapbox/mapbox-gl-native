#include "../fixtures/util.hpp"

#include <mbgl/annotation/point_annotation.hpp>
#include <mbgl/annotation/shape_annotation.hpp>
#include <mbgl/style/style_properties.hpp>
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

TEST(Annotations, LineAnnotation) {
    using namespace mbgl;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    AnnotationSegments segments = {{ {{ { 0, 0 }, { 45, 45 } }} }};

    LineProperties lineProperties;
    lineProperties.color = {{ 255, 0, 0, 1 }};
    lineProperties.width = 5;

    StyleProperties styleProperties;
    styleProperties.set<LineProperties>(lineProperties);

    map.addShapeAnnotation(ShapeAnnotation(segments, styleProperties));

    std::promise<std::unique_ptr<const StillImage>> promise;
    map.renderStill([&promise](std::exception_ptr, std::unique_ptr<const StillImage> image) {
        promise.set_value(std::move(image));
    });

    auto result = promise.get_future().get();
    const std::string png = util::compress_png(result->width, result->height, result->pixels.get());
    util::write_file("test/output/line_annotation.png", png);
}

TEST(Annotations, FillAnnotation) {
    using namespace mbgl;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    AnnotationSegments segments = {{ {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} }};

    FillProperties fillProperties;
    fillProperties.fill_color = {{ 255, 0, 0, 1 }};

    StyleProperties styleProperties;
    styleProperties.set<FillProperties>(fillProperties);

    map.addShapeAnnotation(ShapeAnnotation(segments, styleProperties));

    std::promise<std::unique_ptr<const StillImage>> promise;
    map.renderStill([&promise](std::exception_ptr, std::unique_ptr<const StillImage> image) {
        promise.set_value(std::move(image));
    });

    auto result = promise.get_future().get();
    const std::string png = util::compress_png(result->width, result->height, result->pixels.get());
    util::write_file("test/output/fill_annotation.png", png);
}
