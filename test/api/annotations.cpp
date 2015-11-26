#include "../fixtures/util.hpp"

#include <mbgl/annotation/point_annotation.hpp>
#include <mbgl/annotation/shape_annotation.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/io.hpp>

#include <future>
#include <vector>

using namespace mbgl;

PremultipliedImage render(Map& map) {
    std::promise<PremultipliedImage> promise;
    map.renderStill([&](std::exception_ptr, PremultipliedImage&& image) {
        promise.set_value(std::move(image));
    });
    return std::move(promise.get_future().get());
}

void checkRendering(Map& map, const char * name) {
    PremultipliedImage actual = render(map);
    test::checkImage(std::string("test/fixtures/annotations/") + name + "/",
                     actual, 0.0002, 0.1);
}

TEST(Annotations, PointAnnotation) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.addPointAnnotation(PointAnnotation({ 0, -20 }, "default_marker"));

    checkRendering(map, "point_annotation");
}

TEST(Annotations, LineAnnotation) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    AnnotationSegments segments = {{ {{ { 0, 0 }, { 45, 45 } }} }};

    LineAnnotationProperties properties;
    properties.color = {{ 255, 0, 0, 1 }};
    properties.width = 5;

    map.addShapeAnnotation(ShapeAnnotation(segments, properties));

    checkRendering(map, "line_annotation");
}

TEST(Annotations, FillAnnotation) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    AnnotationSegments segments = {{ {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} }};

    FillAnnotationProperties properties;
    properties.color = {{ 255, 0, 0, 1 }};

    map.addShapeAnnotation(ShapeAnnotation(segments, properties));

    checkRendering(map, "fill_annotation");
}

TEST(Annotations, StyleSourcedShapeAnnotation) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/annotation.json"), "");

    AnnotationSegments segments = {{ {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} }};

    map.addShapeAnnotation(ShapeAnnotation(segments, "annotation"));

    checkRendering(map, "style_sourced_shape_annotation");
}

TEST(Annotations, AddMultiple) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.addPointAnnotation(PointAnnotation({ 0, -20 }, "default_marker"));

    render(map);

    map.addPointAnnotation(PointAnnotation({ 0, 20 }, "default_marker"));

    checkRendering(map, "add_multiple");
}

TEST(Annotations, NonImmediateAdd) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    render(map);

    AnnotationSegments segments = {{ {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} }};

    FillAnnotationProperties properties;
    properties.color = {{ 255, 0, 0, 1 }};

    map.addShapeAnnotation(ShapeAnnotation(segments, properties));

    checkRendering(map, "non_immediate_add");
}

TEST(Annotations, RemovePoint) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    uint32_t point = map.addPointAnnotation(PointAnnotation({ 0, 0 }, "default_marker"));

    render(map);

    map.removeAnnotation(point);

    checkRendering(map, "remove_point");
}

TEST(Annotations, RemoveShape) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    AnnotationSegments segments = {{ {{ { 0, 0 }, { 45, 45 } }} }};

    LineAnnotationProperties properties;
    properties.color = {{ 255, 0, 0, 1 }};
    properties.width = 5;

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    uint32_t shape = map.addShapeAnnotation(ShapeAnnotation(segments, properties));

    render(map);

    map.removeAnnotation(shape);

    checkRendering(map, "remove_shape");
}

TEST(Annotations, ImmediateRemoveShape) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);
    Map map(view, fileSource, MapMode::Still);

    map.removeAnnotation(map.addShapeAnnotation(ShapeAnnotation({}, {})));
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    render(map);
}

TEST(Annotations, SwitchStyle) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.addPointAnnotation(PointAnnotation({ 0, -20 }, "default_marker"));

    render(map);

    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    checkRendering(map, "switch_style");
}

TEST(Annotations, CustomIcon) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.setSprite("cafe", std::make_shared<SpriteImage>(12, 12, 1, std::string(12 * 12 * 4, '\xFF')));
    map.addPointAnnotation(PointAnnotation({ 0, 0 }, "cafe"));

    checkRendering(map, "custom_icon");
}
