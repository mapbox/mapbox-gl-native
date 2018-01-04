#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>

#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/gl/headless_frontend.hpp>

using namespace mbgl;

namespace {

PremultipliedImage namedImage(const std::string& name) {
    return decodeImage(util::read_file("test/fixtures/sprites/" + name + ".png"));
}

std::unique_ptr<style::Image> namedMarker(const std::string& name) {
    return std::make_unique<style::Image>(name, namedImage(name), 1.0);
}

class AnnotationTest {
public:
    util::RunLoop loop;
    StubFileSource fileSource;
    ThreadPool threadPool { 4 };
    float pixelRatio { 1 };
    HeadlessFrontend frontend { pixelRatio, fileSource, threadPool };
    Map map { frontend, MapObserver::nullObserver(), frontend.getSize(), pixelRatio, fileSource,
              threadPool, MapMode::Static};

    void checkRendering(const char * name) {
        test::checkImage(std::string("test/fixtures/annotations/") + name,
                         frontend.render(map), 0.0002, 0.1);
    }
};

} // end namespace

TEST(Annotations, SymbolAnnotation) {
    AnnotationTest test;

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationImage(namedMarker("default_marker"));
    test.map.addAnnotation(SymbolAnnotation { Point<double>(0, 0), "default_marker" });
    test.checkRendering("point_annotation");

//    auto size = test.frontend.getSize();
//    auto screenBox = ScreenBox { {}, { double(size.width), double(size.height) } };
//    for (uint8_t zoom = test.map.getMinZoom(); zoom <= test.map.getMaxZoom(); ++zoom) {
//        test.map.setZoom(zoom);
//        test.checkRendering("point_annotation");
//        EXPECT_EQ(test.map.queryPointAnnotations(screenBox).size(), 1u);
//    }
}

TEST(Annotations, LineAnnotation) {
    AnnotationTest test;

    LineString<double> line = {{ { 0, 0 }, { 45, 45 }, { 30, 0 } }};
    LineAnnotation annotation { line };
    annotation.color = Color::red();
    annotation.width = { 5 };

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotation(annotation);
    test.checkRendering("line_annotation");

    test.map.setZoom(test.map.getMaxZoom());
    test.checkRendering("line_annotation_max_zoom");
}

TEST(Annotations, FillAnnotation) {
    AnnotationTest test;

    Polygon<double> polygon = { {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} };
    FillAnnotation annotation { polygon };
    annotation.color = Color::red();

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotation(annotation);
    test.checkRendering("fill_annotation");

    test.map.setZoom(test.map.getMaxZoom());
    test.checkRendering("fill_annotation_max_zoom");
}

TEST(Annotations, AntimeridianAnnotationSmall) {
    AnnotationTest test;

    double antimeridian = 180;
    test.map.setLatLngZoom(mbgl::LatLng(0, antimeridian), 0);
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));

    LineString<double> line = {{ { antimeridian, 20 }, { antimeridian, -20 } }};
    LineAnnotation lineAnnotation { line };
    lineAnnotation.color = Color::red();
    lineAnnotation.width = { 2 };
    test.map.addAnnotation(lineAnnotation);

    Polygon<double> polygon = { {{ { antimeridian+10, 0 }, { antimeridian - 10, 10 }, { antimeridian-10, -10 } }} };
    FillAnnotation polygonAnnotation { polygon };
    polygonAnnotation.color = Color::blue();
    test.map.addAnnotation(polygonAnnotation);

    test.checkRendering("antimeridian_annotation_small");
}

TEST(Annotations, AntimeridianAnnotationLarge) {
    AnnotationTest test;

    double antimeridian = 180;
    test.map.setLatLngZoom(mbgl::LatLng(0, antimeridian), 0);
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));

    LineString<double> line = {{ { antimeridian, 20 }, { antimeridian, -20 } }};
    LineAnnotation lineAnnotation { line };
    lineAnnotation.color = Color::red();
    lineAnnotation.width = { 2 };
    test.map.addAnnotation(lineAnnotation);

    Polygon<double> polygon = { {{ { antimeridian-10, 0 }, { -antimeridian+10, 10 }, { -antimeridian+10, -10 } }} };
    FillAnnotation polygonAnnotation { polygon };
    polygonAnnotation.color = Color::blue();
    test.map.addAnnotation(polygonAnnotation);

    test.checkRendering("antimeridian_annotation_large");
}

TEST(Annotations, OverlappingFillAnnotation) {
    AnnotationTest test;

    Polygon<double> polygon = { {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} };
    FillAnnotation underlaidAnnotation { polygon };
    underlaidAnnotation.color = Color::green();
    FillAnnotation overlaidAnnotation { polygon };
    overlaidAnnotation.color = Color::red();

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotation(underlaidAnnotation);
    test.map.addAnnotation(overlaidAnnotation);
    test.checkRendering("overlapping_fill_annotation");
}

TEST(Annotations, AddMultiple) {
    AnnotationTest test;

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationImage(namedMarker("default_marker"));
    test.map.addAnnotation(SymbolAnnotation { Point<double> { -10, 0 }, "default_marker" });

    test.frontend.render(test.map);

    test.map.addAnnotation(SymbolAnnotation { Point<double> { 10, 0 }, "default_marker" });
    test.checkRendering("add_multiple");
}

TEST(Annotations, NonImmediateAdd) {
    AnnotationTest test;

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.frontend.render(test.map);

    Polygon<double> polygon = { {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} };
    FillAnnotation annotation { polygon };
    annotation.color = Color::red();

    test.map.addAnnotation(annotation);
    test.checkRendering("non_immediate_add");
}

TEST(Annotations, UpdateSymbolAnnotationGeometry) {
    AnnotationTest test;

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationImage(namedMarker("default_marker"));
    test.map.addAnnotationImage(namedMarker("flipped_marker"));
    AnnotationID point = test.map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "default_marker" });

    test.frontend.render(test.map);

    test.map.updateAnnotation(point, SymbolAnnotation { Point<double> { -10, 0 }, "default_marker" });
    test.checkRendering("update_point");
}

TEST(Annotations, UpdateSymbolAnnotationIcon) {
    AnnotationTest test;

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationImage(namedMarker("default_marker"));
    test.map.addAnnotationImage(namedMarker("flipped_marker"));
    AnnotationID point = test.map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "default_marker" });

    test.frontend.render(test.map);

    test.map.updateAnnotation(point, SymbolAnnotation { Point<double> { 0, 0 }, "flipped_marker" });
    test.checkRendering("update_icon");
}

TEST(Annotations, UpdateLineAnnotationGeometry) {
    AnnotationTest test;

    LineAnnotation annotation { LineString<double> {{ { 0, 0 }, { 45, 45 }, { 30, 0 } }} };
    annotation.color = Color::red();
    annotation.width = { 5 };

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    AnnotationID line = test.map.addAnnotation(annotation);

    test.frontend.render(test.map);

    annotation.geometry = LineString<double> {{ { 0, 0 }, { -45, -45 } }};
    test.map.updateAnnotation(line, annotation);
    test.checkRendering("update_line_geometry");
}

TEST(Annotations, UpdateLineAnnotationStyle) {
    AnnotationTest test;

    LineAnnotation annotation { LineString<double> {{ { 0, 0 }, { 45, 45 }, { 30, 0 } }} };
    annotation.color = Color::red();
    annotation.width = { 5 };

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    AnnotationID line = test.map.addAnnotation(annotation);

    test.frontend.render(test.map);

    annotation.color = Color::green();
    annotation.width = { 2 };
    test.map.updateAnnotation(line, annotation);
    test.checkRendering("update_line_style");
}

TEST(Annotations, UpdateFillAnnotationGeometry) {
    AnnotationTest test;

    FillAnnotation annotation { Polygon<double> { {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} } };
    annotation.color = Color::red();

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    AnnotationID fill = test.map.addAnnotation(annotation);

    test.frontend.render(test.map);

    annotation.geometry = Polygon<double> { {{ { 0, 0 }, { 0, 45 }, { 45, 0 } }} };
    test.map.updateAnnotation(fill, annotation);
    test.checkRendering("update_fill_geometry");
}

TEST(Annotations, UpdateFillAnnotationStyle) {
    AnnotationTest test;

    Polygon<double> polygon = { {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} };
    FillAnnotation annotation { polygon };
    annotation.color = Color::red();

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    AnnotationID fill = test.map.addAnnotation(annotation);

    test.frontend.render(test.map);

    annotation.color = Color::green();
    test.map.updateAnnotation(fill, annotation);
    test.checkRendering("update_fill_style");
}

TEST(Annotations, RemovePoint) {
    AnnotationTest test;

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationImage(namedMarker("default_marker"));
    AnnotationID point = test.map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "default_marker" });

    test.frontend.render(test.map);

    test.map.removeAnnotation(point);
    test.checkRendering("remove_point");
}

TEST(Annotations, RemoveShape) {
    AnnotationTest test;

    LineString<double> line = {{ { 0, 0 }, { 45, 45 } }};
    LineAnnotation annotation { line };
    annotation.color = Color::red();
    annotation.width = { 5 };

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    AnnotationID shape = test.map.addAnnotation(annotation);

    test.frontend.render(test.map);

    test.map.removeAnnotation(shape);
    test.checkRendering("remove_shape");
}

TEST(Annotations, ImmediateRemoveShape) {
    AnnotationTest test;

    test.map.removeAnnotation(test.map.addAnnotation(LineAnnotation { LineString<double>() }));
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));

    test.frontend.render(test.map);
}

TEST(Annotations, SwitchStyle) {
    AnnotationTest test;

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationImage(namedMarker("default_marker"));
    test.map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "default_marker" });

    test.frontend.render(test.map);

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.checkRendering("switch_style");
}

TEST(Annotations, ReaddImage) {
    AnnotationTest test;

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationImage(namedMarker("default_marker"));
    test.map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "default_marker" });

    test.frontend.render(test.map);

    test.map.addAnnotationImage(std::make_unique<style::Image>("default_marker", namedImage("flipped_marker"), 1.0));
    test.checkRendering("readd_image");
}

TEST(Annotations, QueryRenderedFeatures) {
    AnnotationTest test;

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationImage(namedMarker("default_marker"));
    test.map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "default_marker" });
    test.map.addAnnotation(SymbolAnnotation { Point<double> { 0, 50 }, "default_marker" });

    test.frontend.render(test.map);

    auto features = test.frontend.getRenderer()->queryRenderedFeatures(test.map.pixelForLatLng({ 0, 0 }));
    EXPECT_EQ(features.size(), 1u);
    EXPECT_TRUE(!!features[0].id);
    EXPECT_EQ(*features[0].id, uint64_t(0));

    auto features2 = test.frontend.getRenderer()->queryRenderedFeatures(test.map.pixelForLatLng({ 50, 0 }));
    EXPECT_EQ(features2.size(), 1u);
    EXPECT_TRUE(!!features2[0].id);
    EXPECT_EQ(*features2[0].id, uint64_t(1));
}

TEST(Annotations, QueryFractionalZoomLevels) {
    AnnotationTest test;

    auto viewSize = test.frontend.getSize();
    auto box = ScreenBox { {}, { double(viewSize.width), double(viewSize.height) } };

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationImage(namedMarker("default_marker"));

    std::vector<mbgl::AnnotationID> ids;
    for (int longitude = 0; longitude < 10; longitude += 2) {
        for (int latitude = 0; latitude < 10; latitude += 2) {
            ids.push_back(test.map.addAnnotation(SymbolAnnotation { { double(latitude), double(longitude) }, "default_marker" }));
        }
    }

    test.map.setLatLngZoom({ 5, 5 }, 0);
    for (uint16_t zoomSteps = 10; zoomSteps <= 20; ++zoomSteps) {
        test.map.setZoom(zoomSteps / 10.0);
        test.frontend.render(test.map);
        auto features = test.frontend.getRenderer()->queryRenderedFeatures(box);

        // Filter out repeated features.
        // See 'edge-cases/null-island' query-test for reference.
        auto sortID = [](const Feature& lhs, const Feature& rhs) { return lhs.id < rhs.id; };
        auto sameID = [](const Feature& lhs, const Feature& rhs) { return lhs.id == rhs.id; };
        std::sort(features.begin(), features.end(), sortID);
        features.erase(std::unique(features.begin(), features.end(), sameID), features.end());
        EXPECT_EQ(features.size(), ids.size());
    }
}

TEST(Annotations, VisibleFeatures) {
    AnnotationTest test;

    auto viewSize = test.frontend.getSize();
    auto box = ScreenBox { {}, { double(viewSize.width), double(viewSize.height) } };

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotationImage(namedMarker("default_marker"));
    test.map.setLatLngZoom({ 5, 5 }, 3);

    std::vector<mbgl::AnnotationID> ids;
    for (int longitude = 0; longitude < 10; longitude += 2) {
        for (int latitude = 0; latitude <= 10; latitude += 2) {
            ids.push_back(test.map.addAnnotation(SymbolAnnotation { { double(latitude), double(longitude) }, "default_marker" }));
        }
    }

    // Change bearing *after* adding annotations causes them to be reordered.
    test.map.setBearing(45);
    test.frontend.render(test.map);

    auto features = test.frontend.getRenderer()->queryRenderedFeatures(box, {});
    auto sortID = [](const Feature& lhs, const Feature& rhs) { return lhs.id < rhs.id; };
    auto sameID = [](const Feature& lhs, const Feature& rhs) { return lhs.id == rhs.id; };
    std::sort(features.begin(), features.end(), sortID);
    features.erase(std::unique(features.begin(), features.end(), sameID), features.end());
    EXPECT_EQ(features.size(), ids.size());

    test.map.setBearing(0);
    test.map.setZoom(4);
    test.frontend.render(test.map);
    features = test.frontend.getRenderer()->queryRenderedFeatures(box);
    std::sort(features.begin(), features.end(), sortID);
    features.erase(std::unique(features.begin(), features.end(), sameID), features.end());
    EXPECT_EQ(features.size(), ids.size());
}

TEST(Annotations, TopOffsetPixels) {
    AnnotationTest test;

    test.map.addAnnotationImage(namedMarker("default_marker"));
    EXPECT_EQ(test.map.getTopOffsetPixelsForAnnotationImage("default_marker"), -28);
}

TEST(Annotations, DebugEmpty) {
    // This test should render nothing, not even the tile borders. Tile borders are only rendered
    // when there is an actual tile we're trying to render, but since there is no annotation, we
    // should not render them.
    AnnotationTest test;

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.setDebug(MapDebugOptions::TileBorders);
    test.map.setZoom(1);

    test.checkRendering("debug_empty");
}


TEST(Annotations, DebugSparse) {
    // This test should only render the top right tile with the associated tile border, but no other
    // tiles because they're all empty.
    AnnotationTest test;

    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.setDebug(MapDebugOptions::TileBorders);
    test.map.setZoom(1);
    test.map.addAnnotationImage(namedMarker("default_marker"));
    test.map.addAnnotation(SymbolAnnotation { Point<double>(10, 10), "default_marker" });

    test.checkRendering("debug_sparse");
}

TEST(Annotations, ChangeMaxZoom) {
    AnnotationTest test;

    LineString<double> line = {{ { 0, 0 }, { 45, 45 }, { 30, 0 } }};
    LineAnnotation annotation { line };
    annotation.color = Color::red();
    annotation.width = { 5 };

    test.map.setMaxZoom(6);
    test.map.getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
    test.map.addAnnotation(annotation);
    test.map.setMaxZoom(14);
    test.map.setZoom(test.map.getMaxZoom());
    test.checkRendering("line_annotation_max_zoom");
}
