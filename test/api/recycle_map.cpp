#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/map_adapter.hpp>

#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/geojson.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>

using namespace mbgl;
using namespace mbgl::style;


TEST(API, RecycleMapUpdateImages) {
    util::RunLoop loop;

    HeadlessFrontend frontend { 1 };
    auto map = std::make_unique<MapAdapter>(frontend, MapObserver::nullObserver(),
                                         std::make_shared<StubFileSource>(),
                                         MapOptions().withMapMode(MapMode::Static).withSize(frontend.getSize()));

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
    test::checkImage("test/fixtures/recycle_map/default_marker", frontend.render(*map).image, 0.0006, 0.1);

    // flipped marker

    loadStyle("flipped_marker", "test/fixtures/sprites/flipped_marker.png");
    test::checkImage("test/fixtures/recycle_map/flipped_marker", frontend.render(*map).image, 0.0006, 0.1);
}
