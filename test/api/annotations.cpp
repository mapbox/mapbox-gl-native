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

namespace {

std::string getFileSourceRoot() {
#ifdef MBGL_ASSET_ZIP
    return "test/fixtures/annotations/assets.zip";
#else
    return "";
#endif
}

}

std::shared_ptr<SpriteImage> defaultMarker() {
    PremultipliedImage image = decodeImage(util::read_file("test/fixtures/sprites/default_marker.png"));
    return std::make_shared<SpriteImage>(image.width, image.height, 1.0, std::string(reinterpret_cast<char*>(image.data.get()), image.size()));
}

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
    DefaultFileSource fileSource(nullptr, getFileSourceRoot());

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.setSprite("default_marker", defaultMarker());
    map.addPointAnnotation(PointAnnotation({ 0, 0 }, "default_marker"));

    checkRendering(map, "point_annotation");
}

TEST(Annotations, LineAnnotation) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr, getFileSourceRoot());

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
    DefaultFileSource fileSource(nullptr, getFileSourceRoot());

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
    DefaultFileSource fileSource(nullptr, getFileSourceRoot());

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/annotation.json"), "");

    AnnotationSegments segments = {{ {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} }};

    map.addShapeAnnotation(ShapeAnnotation(segments, "annotation"));

    checkRendering(map, "style_sourced_shape_annotation");
}

TEST(Annotations, AddMultiple) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr, getFileSourceRoot());

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.setSprite("default_marker", defaultMarker());
    map.addPointAnnotation(PointAnnotation({ 0, -10 }, "default_marker"));

    render(map);

    map.addPointAnnotation(PointAnnotation({ 0, 10 }, "default_marker"));

    checkRendering(map, "add_multiple");
}

TEST(Annotations, NonImmediateAdd) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr, getFileSourceRoot());

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
    DefaultFileSource fileSource(nullptr, getFileSourceRoot());

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.setSprite("default_marker", defaultMarker());
    uint32_t point = map.addPointAnnotation(PointAnnotation({ 0, 0 }, "default_marker"));

    render(map);

    map.removeAnnotation(point);

    checkRendering(map, "remove_point");
}

TEST(Annotations, RemoveShape) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr, getFileSourceRoot());

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
    DefaultFileSource fileSource(nullptr, getFileSourceRoot());
    Map map(view, fileSource, MapMode::Still);

    map.removeAnnotation(map.addShapeAnnotation(ShapeAnnotation({}, {})));
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    render(map);
}

TEST(Annotations, SwitchStyle) {
    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr, getFileSourceRoot());

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.setSprite("default_marker", defaultMarker());
    map.addPointAnnotation(PointAnnotation({ 0, 0 }, "default_marker"));

    render(map);

    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    checkRendering(map, "switch_style");
}
