#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/color.hpp>

using namespace mbgl;

namespace {

std::shared_ptr<SpriteImage> namedMarker(const std::string &name) {
    PremultipliedImage image = decodeImage(util::read_file("test/fixtures/sprites/" + name));
    return std::make_shared<SpriteImage>(std::move(image), 1.0);
}

class AnnotationTest {
public:
    util::RunLoop loop;
    std::shared_ptr<HeadlessDisplay> display { std::make_shared<HeadlessDisplay>() };
    HeadlessView view { display, 1 };
    StubFileSource fileSource;
    Map map { view, fileSource, MapMode::Still };

    void checkRendering(const char * name) {
        test::checkImage(std::string("test/fixtures/annotations/") + name,
                         test::render(map), 0.0002, 0.1);
    }
};

} // end namespace

TEST(Annotations, SymbolAnnotation) {
    AnnotationTest test;

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationIcon("default_marker", namedMarker("default_marker.png"));
    test.map.addAnnotation(SymbolAnnotation { Point<double>(0, 0), "default_marker" });
    test.checkRendering("point_annotation");

    // FIXME: https://github.com/mapbox/mapbox-gl-native/issues/5419
    //test.map.setZoom(test.map.getMaxZoom());
    //test.checkRendering("point_annotation");
}

TEST(Annotations, LineAnnotation) {
    AnnotationTest test;

    LineString<double> line = {{ { 0, 0 }, { 45, 45 }, { 30, 0 } }};
    LineAnnotation annotation { line };
    annotation.color = { { 255, 0, 0, 1 } };
    annotation.width = { 5 };

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotation(annotation);
    test.checkRendering("line_annotation");

    test.map.setZoom(test.map.getMaxZoom());
    test.checkRendering("line_annotation_max_zoom");
}

TEST(Annotations, FillAnnotation) {
    AnnotationTest test;

    Polygon<double> polygon = {{ {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} }};
    FillAnnotation annotation { polygon };
    annotation.color = { { 255, 0, 0, 1 } };

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotation(annotation);
    test.checkRendering("fill_annotation");

    test.map.setZoom(test.map.getMaxZoom());
    test.checkRendering("fill_annotation_max_zoom");
}

TEST(Annotations, AntimeridianAnnotationSmall) {
    AnnotationTest test;

    double antimeridian = 180;
    test.map.setLatLngZoom(mbgl::LatLng(0, antimeridian), 0);
    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));

    LineString<double> line = {{ { antimeridian, 20 }, { antimeridian, -20 } }};
    LineAnnotation lineAnnotation { line };
    lineAnnotation.color = { { 255, 0, 0, 1 } };
    lineAnnotation.width = { 2 };
    test.map.addAnnotation(lineAnnotation);

    Polygon<double> polygon = {{ {{ { antimeridian+10, 0 }, { antimeridian - 10, 10 }, { antimeridian-10, -10 } }} }};
    FillAnnotation polygonAnnotation { polygon };
    polygonAnnotation.color = { { 0, 0, 255, 1 } };
    test.map.addAnnotation(polygonAnnotation);

    test.checkRendering("antimeridian_annotation_small");
}

TEST(Annotations, AntimeridianAnnotationLarge) {
    AnnotationTest test;

    double antimeridian = 180;
    test.map.setLatLngZoom(mbgl::LatLng(0, antimeridian), 0);
    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));

    LineString<double> line = {{ { antimeridian, 20 }, { antimeridian, -20 } }};
    LineAnnotation lineAnnotation { line };
    lineAnnotation.color = { { 255, 0, 0, 1 } };
    lineAnnotation.width = { 2 };
    test.map.addAnnotation(lineAnnotation);

    Polygon<double> polygon = {{ {{ { antimeridian-10, 0 }, { -antimeridian+10, 10 }, { -antimeridian+10, -10 } }} }};
    FillAnnotation polygonAnnotation { polygon };
    polygonAnnotation.color = { { 0, 0, 255, 1 } };
    test.map.addAnnotation(polygonAnnotation);

    test.checkRendering("antimeridian_annotation_large");
}

TEST(Annotations, OverlappingFillAnnotation) {
    AnnotationTest test;

    Polygon<double> polygon = {{ {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} }};
    FillAnnotation underlaidAnnotation { polygon };
    underlaidAnnotation.color = { { 0, 255, 0, 1 } };
    FillAnnotation overlaidAnnotation { polygon };
    overlaidAnnotation.color = { { 255, 0, 0, 1 } };

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotation(underlaidAnnotation);
    test.map.addAnnotation(overlaidAnnotation);
    test.checkRendering("overlapping_fill_annotation");
}

TEST(Annotations, StyleSourcedShapeAnnotation) {
    AnnotationTest test;

    Polygon<double> polygon = {{ {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} }};

    test.map.setStyleJSON(util::read_file("test/fixtures/api/annotation.json"));
    test.map.addAnnotation(StyleSourcedAnnotation { polygon, "annotation" });
    test.checkRendering("style_sourced_shape_annotation");
}

TEST(Annotations, AddMultiple) {
    AnnotationTest test;

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationIcon("default_marker", namedMarker("default_marker.png"));
    test.map.addAnnotation(SymbolAnnotation { Point<double> { -10, 0 }, "default_marker" });

    test::render(test.map);

    test.map.addAnnotation(SymbolAnnotation { Point<double> { 10, 0 }, "default_marker" });
    test.checkRendering("add_multiple");
}

TEST(Annotations, NonImmediateAdd) {
    AnnotationTest test;

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    test::render(test.map);

    Polygon<double> polygon = {{ {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} }};
    FillAnnotation annotation { polygon };
    annotation.color = { { 255, 0, 0, 1 } };

    test.map.addAnnotation(annotation);
    test.checkRendering("non_immediate_add");
}

TEST(Annotations, UpdateSymbolAnnotationGeometry) {
    AnnotationTest test;

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationIcon("default_marker", namedMarker("default_marker.png"));
    test.map.addAnnotationIcon("flipped_marker", namedMarker("flipped_marker.png"));
    AnnotationID point = test.map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "default_marker" });

    test::render(test.map);

    test.map.updateAnnotation(point, SymbolAnnotation { Point<double> { -10, 0 }, "default_marker" });
    test.checkRendering("update_point");
}

TEST(Annotations, UpdateSymbolAnnotationIcon) {
    AnnotationTest test;

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationIcon("default_marker", namedMarker("default_marker.png"));
    test.map.addAnnotationIcon("flipped_marker", namedMarker("flipped_marker.png"));
    AnnotationID point = test.map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "default_marker" });

    test::render(test.map);

    test.map.updateAnnotation(point, SymbolAnnotation { Point<double> { 0, 0 }, "flipped_marker" });
    test.checkRendering("update_icon");
}

TEST(Annotations, UpdateLineAnnotationGeometry) {
    AnnotationTest test;

    LineAnnotation annotation { LineString<double> {{ { 0, 0 }, { 45, 45 }, { 30, 0 } }} };
    annotation.color = { { 255, 0, 0, 1 } };
    annotation.width = { 5 };

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    AnnotationID line = test.map.addAnnotation(annotation);

    test::render(test.map);

    annotation.geometry = LineString<double> {{ { 0, 0 }, { -45, -45 } }};
    test.map.updateAnnotation(line, annotation);
    test.checkRendering("update_line_geometry");
}

TEST(Annotations, UpdateLineAnnotationStyle) {
    AnnotationTest test;

    LineAnnotation annotation { LineString<double> {{ { 0, 0 }, { 45, 45 }, { 30, 0 } }} };
    annotation.color = { { 255, 0, 0, 1 } };
    annotation.width = { 5 };

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    AnnotationID line = test.map.addAnnotation(annotation);

    test::render(test.map);

    annotation.color = { { 0, 255, 0, 1 } };
    annotation.width = { 2 };
    test.map.updateAnnotation(line, annotation);
    test.checkRendering("update_line_style");
}

TEST(Annotations, UpdateFillAnnotationGeometry) {
    AnnotationTest test;

    FillAnnotation annotation { Polygon<double> {{ {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} }} };
    annotation.color = { { 255, 0, 0, 1 } };

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    AnnotationID fill = test.map.addAnnotation(annotation);

    test::render(test.map);

    annotation.geometry = Polygon<double> {{ {{ { 0, 0 }, { 0, 45 }, { 45, 0 } }} }};
    test.map.updateAnnotation(fill, annotation);
    test.checkRendering("update_fill_geometry");
}

TEST(Annotations, UpdateFillAnnotationStyle) {
    AnnotationTest test;

    Polygon<double> polygon = {{ {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} }};
    FillAnnotation annotation { polygon };
    annotation.color = { { 255, 0, 0, 1 } };

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    AnnotationID fill = test.map.addAnnotation(annotation);

    test::render(test.map);

    annotation.color = { { 0, 255, 0, 1 } };
    test.map.updateAnnotation(fill, annotation);
    test.checkRendering("update_fill_style");
}

TEST(Annotations, RemovePoint) {
    AnnotationTest test;

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationIcon("default_marker", namedMarker("default_marker.png"));
    AnnotationID point = test.map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "default_marker" });

    test::render(test.map);

    test.map.removeAnnotation(point);
    test.checkRendering("remove_point");
}

TEST(Annotations, RemoveShape) {
    AnnotationTest test;

    LineString<double> line = {{ { 0, 0 }, { 45, 45 } }};
    LineAnnotation annotation { line };
    annotation.color = { { 255, 0, 0, 1 } };
    annotation.width = { 5 };

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    AnnotationID shape = test.map.addAnnotation(annotation);

    test::render(test.map);

    test.map.removeAnnotation(shape);
    test.checkRendering("remove_shape");
}

TEST(Annotations, ImmediateRemoveShape) {
    AnnotationTest test;

    test.map.removeAnnotation(test.map.addAnnotation(LineAnnotation { LineString<double>() }));
    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));

    test::render(test.map);
}

TEST(Annotations, SwitchStyle) {
    AnnotationTest test;

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationIcon("default_marker", namedMarker("default_marker.png"));
    test.map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "default_marker" });

    test::render(test.map);

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    test.checkRendering("switch_style");
}

TEST(Annotations, QueryRenderedFeatures) {
    AnnotationTest test;

    test.map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationIcon("default_marker", namedMarker("default_marker.png"));
    test.map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "default_marker" });
    test.map.addAnnotation(SymbolAnnotation { Point<double> { 0, 50 }, "default_marker" });

    test::render(test.map);

    auto features = test.map.queryRenderedFeatures(test.map.pixelForLatLng({ 0, 0 }));
    EXPECT_EQ(features.size(), 1u);
    EXPECT_TRUE(!!features[0].id);
    EXPECT_EQ(*features[0].id, 0);

    auto features2 = test.map.queryRenderedFeatures(test.map.pixelForLatLng({ 50, 0 }));
    EXPECT_EQ(features2.size(), 1u);
    EXPECT_TRUE(!!features2[0].id);
    EXPECT_EQ(*features2[0].id, 1);
}
