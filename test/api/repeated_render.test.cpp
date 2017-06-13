#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/map/backend_scope.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/offscreen_view.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/sources/geojson_source.hpp>

#include <future>

using namespace mbgl;


TEST(API, RepeatedRender) {

    util::RunLoop loop;

    const auto style = util::read_file("test/fixtures/api/water.json");

    HeadlessBackend backend { test::sharedDisplay() };
    BackendScope scope { backend };
    OffscreenView view { backend.getContext(), { 512, 512 } };
    DefaultFileSource fileSource(":memory:", "test/fixtures/api/assets");
    ThreadPool threadPool(4);

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    Map map(backend, MapObserver::nullObserver(), view.getSize(), 1, fileSource, threadPool, MapMode::Still);

    {
        map.getStyle().loadJSON(style);
        PremultipliedImage result;
        map.renderStill(view, [&](std::exception_ptr) {
            result = view.readStillImage();
        });

        while (!result.valid()) {
            loop.runOnce();
        }

        ASSERT_EQ(512u, result.size.width);
        ASSERT_EQ(512u, result.size.height);
        test::checkImage("test/fixtures/api/repeated_render", result, 0.0003, 0.1);
    }

    {
        map.getStyle().loadJSON(style);
        PremultipliedImage result;
        map.renderStill(view, [&](std::exception_ptr) {
            result = view.readStillImage();
        });

        while (!result.valid()) {
            loop.runOnce();
        }

        ASSERT_EQ(512u, result.size.width);
        ASSERT_EQ(512u, result.size.height);
        test::checkImage("test/fixtures/api/repeated_render", result, 0.0003, 0.1);
    }

    auto observer = Log::removeObserver();
    auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    auto unchecked = flo->unchecked();
    EXPECT_TRUE(unchecked.empty()) << unchecked;
}

TEST(API, ZoomHistory) {
    util::RunLoop loop;

    const auto style = util::read_file("test/fixtures/api/empty.json");

    HeadlessBackend backend { test::sharedDisplay() };
    BackendScope scope { backend };
    OffscreenView view { backend.getContext(), { 512, 512 } };
    DefaultFileSource fileSource(":memory:", ".");
    ThreadPool threadPool(4);

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    Map map(backend, MapObserver::nullObserver(), view.getSize(), 1, fileSource, threadPool, MapMode::Still);
    map.getStyle().loadJSON(style);

    auto geojson = mapbox::geojson::parse(R"t({ "type": "FeatureCollection", "features": [{ "type": "Feature", "properties": {}, "geometry": { "type": "LineString", "coordinates": [ [ -150, -75 ], [  150,  75 ] ] } } ] })t");
    auto source = std::make_unique<mbgl::style::GeoJSONSource>("testSource");
    source->setGeoJSON(std::move(geojson));
    map.getStyle().addSource(std::move(source));

    auto layer = std::make_unique<mbgl::style::LineLayer>("testLayer", "testSource");
    layer->setLineDasharray({ { 1.0f, 2.0f } });
    layer->setLineWidth({ 16.0f });
    map.getStyle().addLayer(std::move(layer));

    {
        PremultipliedImage result;
        map.renderStill(view, [&](std::exception_ptr) {
            result = view.readStillImage();
        });

        while (!result.valid()) {
            loop.runOnce();
        }

        test::checkImage("test/fixtures/api/z0", result, 0.0002, 0.1);
    }

    {
        map.setZoom(1.0);

        PremultipliedImage result;
        map.renderStill(view, [&](std::exception_ptr) {
            result = view.readStillImage();
        });

        while (!result.valid()) {
            loop.runOnce();
        }

        test::checkImage("test/fixtures/api/z1", result, 0.0002, 0.1);
    }

    auto observer = Log::removeObserver();
    auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    auto unchecked = flo->unchecked();
    EXPECT_TRUE(unchecked.empty()) << unchecked;
}
