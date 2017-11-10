#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>

#include <mbgl/gl/headless_frontend.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/style/function/camera_function.hpp>
#include <mbgl/style/function/exponential_stops.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/geojson.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>

using namespace mbgl;
using namespace mbgl::style;

TEST(API, ZoomHistory) {
    util::RunLoop loop;

    StubFileSource fileSource;
    ThreadPool threadPool(4);
    float pixelRatio { 1 };

    HeadlessFrontend frontend { pixelRatio, fileSource, threadPool };
    auto map = std::make_unique<Map>(frontend, MapObserver::nullObserver(), frontend.getSize(),
                                     pixelRatio, fileSource, threadPool, MapMode::Static);

    EXPECT_TRUE(map);

    map->getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));

    auto source = std::make_unique<GeoJSONSource>("source");
    source->setGeoJSON({ LineString<double> { { 45, -45 }, { -45, 45 } } });
    map->getStyle().addSource(std::move(source));

    auto layer = std::make_unique<LineLayer>("layer-1", "source");
    layer->setLineWidth(CameraFunction<float> { ExponentialStops<float> {{ { 0, 8 }, { 1, 16 } }} });
    layer->setLineDasharray({ std::vector<float> { 1, 1 } });
    layer->setLineColor( { Color::black() } );
    map->getStyle().addLayer(std::move(layer));

    layer = std::make_unique<LineLayer>("layer-2", "source");
    layer->setLineWidth(CameraFunction<float> { ExponentialStops<float> {{ { 0, 4 }, { 1, 8 } }} });
    layer->setLineDasharray({ std::vector<float> { 2, 2 } });
    layer->setLineColor( { Color::red() } );
    map->getStyle().addLayer(std::move(layer));

    // ZoomHistory.lastIntegerZoom is 1.
    map->setZoom(1.0);
    frontend.render(*map);

    map->setZoom(0.0);
    frontend.render(*map);

    // ZoomHistory.lastIntegerZoom should be 0.
    map->setZoom(0.5);
    test::checkImage("test/fixtures/zoom_history", frontend.render(*map), 0.0002);

    map->setZoom(1.0);
    frontend.render(*map);

    map->setZoom(0.5);
    test::checkImage("test/fixtures/zoom_history", frontend.render(*map), 0.0002);
}
