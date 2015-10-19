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

using namespace mbgl;

std::string renderPNG(Map& map) {
    std::promise<std::unique_ptr<const StillImage>> promise;
    map.renderStill([&](std::exception_ptr, std::unique_ptr<const StillImage> image) {
        promise.set_value(std::move(image));
    });

    auto result = promise.get_future().get();
    return util::compress_png(result->width, result->height, result->pixels.get());
}

TEST(Annotations, PointAnnotation) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.addPointAnnotation(PointAnnotation({ 0, 0 }, "default_marker"));

    util::write_file("test/output/point_annotation.png", renderPNG(map));
}

TEST(Annotations, LineAnnotation) {
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

    util::write_file("test/output/line_annotation.png", renderPNG(map));
}

TEST(Annotations, FillAnnotation) {
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

    util::write_file("test/output/fill_annotation.png", renderPNG(map));
}

TEST(Annotations, AddMultiple) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.addPointAnnotation(PointAnnotation({ 0, -20 }, "default_marker"));

    renderPNG(map);

    map.addPointAnnotation(PointAnnotation({ 0, 20 }, "default_marker"));

    util::write_file("test/output/add_multiple.png", renderPNG(map));
}

TEST(Annotations, NonImmediateAdd) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    renderPNG(map);

    AnnotationSegments segments = {{ {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} }};

    FillProperties fillProperties;
    fillProperties.fill_color = {{ 255, 0, 0, 1 }};

    StyleProperties styleProperties;
    styleProperties.set<FillProperties>(fillProperties);

    map.addShapeAnnotation(ShapeAnnotation(segments, styleProperties));

    util::write_file("test/output/non_immediate_add.png", renderPNG(map));
}

TEST(Annotations, RemovePoint) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    uint32_t point = map.addPointAnnotation(PointAnnotation({ 0, 0 }, "default_marker"));

    renderPNG(map);

    map.removeAnnotation(point);

    util::write_file("test/output/remove_point.png", renderPNG(map));
}

TEST(Annotations, RemoveShape) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    AnnotationSegments segments = {{ {{ { 0, 0 }, { 45, 45 } }} }};

    LineProperties lineProperties;
    lineProperties.color = {{ 255, 0, 0, 1 }};
    lineProperties.width = 5;

    StyleProperties styleProperties;
    styleProperties.set<LineProperties>(lineProperties);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    uint32_t shape = map.addShapeAnnotation(ShapeAnnotation(segments, styleProperties));

    renderPNG(map);

    map.removeAnnotation(shape);

    util::write_file("test/output/remove_shape.png", renderPNG(map));
}

TEST(Annotations, SwitchStyle) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.addPointAnnotation(PointAnnotation({ 0, 0 }, "default_marker"));

    renderPNG(map);

    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    util::write_file("test/output/switch_style.png", renderPNG(map));
}
