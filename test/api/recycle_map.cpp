#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>

#include <mbgl/gl/headless_frontend.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/geojson.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>

using namespace mbgl;
using namespace mbgl::style;


TEST(API, RecycleMapUpdateImages) {
    util::RunLoop loop;

    StubFileSource fileSource;
    ThreadPool threadPool(4);
    float pixelRatio { 1 };

    HeadlessFrontend frontend { pixelRatio, fileSource, threadPool };
    auto map = std::make_unique<Map>(frontend, MapObserver::nullObserver(), frontend.getSize(),
                                     pixelRatio, fileSource, threadPool, MapMode::Static);

    EXPECT_TRUE(map);

    auto loadStyle = [&](auto markerName, auto markerPath) {
        auto source = std::make_unique<GeoJSONSource>("geometry");
        source->setGeoJSON( Geometry<double>{ Point<double>{ 0, 0 } } );

        auto layer = std::make_unique<SymbolLayer>("geometry", "geometry");
        layer->setIconImage({ markerName });

        map->getStyle().loadJSON(util::read_file("test/fixtures/api/empty.json"));
        map->getStyle().addSource(std::move(source));
        map->getStyle().addLayer(std::move(layer));
        map->getStyle().addImage(std::make_unique<style::Image>(markerName, decodeImage(util::read_file(markerPath)), 1.0));
    };

    // default marker

    loadStyle("default_marker", "test/fixtures/sprites/default_marker.png");
    test::checkImage("test/fixtures/recycle_map/default_marker", frontend.render(*map), 0.0006, 0.1);

    // flipped marker

    loadStyle("flipped_marker", "test/fixtures/sprites/flipped_marker.png");
    test::checkImage("test/fixtures/recycle_map/flipped_marker", frontend.render(*map), 0.0006, 0.1);
}
