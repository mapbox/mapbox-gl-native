#include <mbgl/test/util.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/util/geojson.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/run_loop.hpp>

using namespace mbgl;
using namespace mbgl::style;

TEST(CustomGeometrySource, Grid) {
    util::RunLoop loop;

    HeadlessFrontend frontend { 1 };
    Map map(frontend, MapObserver::nullObserver(),
            MapOptions().withMapMode(MapMode::Static).withSize(frontend.getSize()),
            ResourceOptions().withCachePath(":memory:").withAssetPath("test/fixtures/api/assets"));
    map.getStyle().loadJSON(util::read_file("test/fixtures/api/water.json"));
    map.jumpTo(CameraOptions().withCenter(LatLng { 37.8, -122.5 }).withZoom(10.0));

    CustomGeometrySource::Options options;
    options.fetchTileFunction = [&map](const mbgl::CanonicalTileID& tileID) {
        double gridSpacing = 0.1;
        FeatureCollection features;
        const LatLngBounds bounds(tileID);
        for (double y = ceil(bounds.north() / gridSpacing) * gridSpacing; y >= floor(bounds.south() / gridSpacing) * gridSpacing; y -= gridSpacing) {

            mapbox::geojson::line_string gridLine;
            gridLine.emplace_back(bounds.west(), y);
            gridLine.emplace_back(bounds.east(), y);

            features.emplace_back(gridLine);
        }

        for (double x = floor(bounds.west() / gridSpacing) * gridSpacing; x <= ceil(bounds.east() / gridSpacing) * gridSpacing; x += gridSpacing) {
            mapbox::geojson::line_string gridLine;
            gridLine.emplace_back(x, bounds.south());
            gridLine.emplace_back(x, bounds.north());
            
            features.emplace_back(gridLine);
        }
        auto source = static_cast<CustomGeometrySource *>(map.getStyle().getSource("custom"));
        if (source) {
            source->setTileData(tileID, features);
        }
    };

    map.getStyle().addSource(std::make_unique<CustomGeometrySource>("custom", options));

    auto fillLayer = std::make_unique<FillLayer>("landcover", "mapbox");
    fillLayer->setSourceLayer("landcover");
    fillLayer->setFillColor(Color{ 1.0, 1.0, 0.0, 1.0 });
    map.getStyle().addLayer(std::move(fillLayer));

    auto layer = std::make_unique<LineLayer>("grid", "custom");
    layer->setLineColor(Color{ 1.0, 1.0, 1.0, 1.0 });
    map.getStyle().addLayer(std::move(layer));

    test::checkImage("test/fixtures/custom_geometry_source/grid", frontend.render(map).image, 0.0006, 0.1);
}
