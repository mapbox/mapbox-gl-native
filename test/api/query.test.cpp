#include <mbgl/map/map.hpp>
#include <mbgl/map/backend_scope.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/offscreen_view.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/source.hpp>

using namespace mbgl;
using namespace mbgl::style;

namespace {

class QueryTest {
public:
    QueryTest() {
        map.getStyle().loadJSON(util::read_file("test/fixtures/api/query_style.json"));
        map.getStyle().addImage(std::make_unique<style::Image>("test-icon",
            decodeImage(util::read_file("test/fixtures/sprites/default_marker.png")), 1.0));

        test::render(map, view);
    }

    util::RunLoop loop;
    HeadlessBackend backend { test::sharedDisplay() };
    BackendScope scope { backend };
    OffscreenView view { backend.getContext() };
    StubFileSource fileSource;
    ThreadPool threadPool { 4 };
    Map map { backend, MapObserver::nullObserver(), view.getSize(), 1, fileSource, threadPool, MapMode::Still };
};

} // end namespace

TEST(Query, QueryRenderedFeatures) {
    QueryTest test;

    auto features1 = test.map.queryRenderedFeatures(test.map.pixelForLatLng({ 0, 0 }));
    EXPECT_EQ(features1.size(), 4u);

    auto features2 = test.map.queryRenderedFeatures(test.map.pixelForLatLng({ 9, 9 }));
    EXPECT_EQ(features2.size(), 0u);
}

TEST(Query, QueryRenderedFeaturesFilterLayer) {
    QueryTest test;

    auto zz = test.map.pixelForLatLng({ 0, 0 });

    auto features1 = test.map.queryRenderedFeatures(zz, {{{ "layer1"}}, {}});
    EXPECT_EQ(features1.size(), 1u);

    auto features2 = test.map.queryRenderedFeatures(zz, {{{ "layer1", "layer2" }}, {}});
    EXPECT_EQ(features2.size(), 2u);

    auto features3 = test.map.queryRenderedFeatures(zz, {{{ "foobar" }}, {}});
    EXPECT_EQ(features3.size(), 0u);

    auto features4 = test.map.queryRenderedFeatures(zz, {{{ "foobar", "layer3" }}, {}});
    EXPECT_EQ(features4.size(), 1u);
}

TEST(Query, QueryRenderedFeaturesFilter) {
    QueryTest test;

    auto zz = test.map.pixelForLatLng({ 0, 0 });

    const EqualsFilter eqFilter = { "key1", std::string("value1") };
    auto features1 = test.map.queryRenderedFeatures(zz, {{}, { eqFilter }});
    EXPECT_EQ(features1.size(), 1u);

    const IdentifierNotEqualsFilter idNotEqFilter = { std::string("feature1") };
    auto features2 = test.map.queryRenderedFeatures(zz, {{{ "layer4" }}, { idNotEqFilter }});
    EXPECT_EQ(features2.size(), 0u);

    const GreaterThanFilter gtFilter = { "key2", 1.0 };
    auto features3 = test.map.queryRenderedFeatures(zz, {{ }, { gtFilter }});
    EXPECT_EQ(features3.size(), 1u);
}

TEST(Query, QuerySourceFeatures) {
    QueryTest test;

    auto features1 = test.map.querySourceFeatures("source3");
    EXPECT_EQ(features1.size(), 1u);
}

TEST(Query, QuerySourceFeaturesOptionValidation) {
    QueryTest test;

    // GeoJSONSource, doesn't require a layer id
    auto features = test.map.querySourceFeatures("source3");
    ASSERT_EQ(features.size(), 1u);

    // VectorSource, requires a layer id
    features = test.map.querySourceFeatures("source5");
    ASSERT_EQ(features.size(), 0u);
    
    // RasterSource, not supported
    features = test.map.querySourceFeatures("source6");
    ASSERT_EQ(features.size(), 0u);
}

TEST(Query, QuerySourceFeaturesFilter) {
    QueryTest test;

    const EqualsFilter eqFilter = { "key1", std::string("value1") };
    auto features1 = test.map.querySourceFeatures("source4", {{}, { eqFilter }});
    EXPECT_EQ(features1.size(), 1u);

    const IdentifierNotEqualsFilter idNotEqFilter = { std::string("feature1") };
    auto features2 = test.map.querySourceFeatures("source4", {{}, { idNotEqFilter }});
    EXPECT_EQ(features2.size(), 0u);

    const GreaterThanFilter gtFilter = { "key2", 1.0 };
    auto features3 = test.map.querySourceFeatures("source4", {{}, { gtFilter }});
    EXPECT_EQ(features3.size(), 1u);
}

