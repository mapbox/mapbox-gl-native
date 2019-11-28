#include <mbgl/test/map_adapter.hpp>

#include <mbgl/map/map_options.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/gfx/headless_frontend.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace mbgl::style::expression;
using namespace std::literals;

namespace {

class QueryTest {
public:
    QueryTest() {
        map.getStyle().loadJSON(util::read_file("test/fixtures/api/query_style.json"));
        map.getStyle().addImage(std::make_unique<style::Image>("test-icon",
            decodeImage(util::read_file("test/fixtures/sprites/default_marker.png")), 1.0));

        frontend.render(map);
    }

    util::RunLoop loop;
    std::shared_ptr<StubFileSource> fileSource = std::make_shared<StubFileSource>();
    HeadlessFrontend frontend { 1 };
    MapAdapter map { frontend, MapObserver::nullObserver(), fileSource,
                  MapOptions().withMapMode(MapMode::Static).withSize(frontend.getSize())};
};

std::vector<Feature> getTopClusterFeature(QueryTest& test) {
    test.fileSource->sourceResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://url"s, resource.url);
        Response response;
        response.data = std::make_unique<std::string>(util::read_file("test/fixtures/supercluster/places.json"s));
        return response;
    };

    LatLng coordinate{0, 0};
    Mutable<GeoJSONOptions> options = makeMutable<GeoJSONOptions>();
    options->cluster = true;
    auto source = std::make_unique<GeoJSONSource>("cluster_source"s, std::move(options));
    source->setURL("http://url"s);
    source->loadDescription(*test.fileSource);

    auto clusterLayer = std::make_unique<SymbolLayer>("cluster_layer"s, "cluster_source"s);
    clusterLayer->setIconImage({"test-icon"s});
    clusterLayer->setIconSize(12.0f);

    test.map.jumpTo(CameraOptions().withCenter(coordinate).withZoom(0.0));
    test.map.getStyle().addSource(std::move(source));
    test.map.getStyle().addLayer(std::move(clusterLayer));
    test.loop.runOnce();
    test.frontend.render(test.map);

    auto screenCoordinate = test.map.pixelForLatLng(coordinate);
    const RenderedQueryOptions queryOptions({{{ "cluster_layer"s}}, {}});
    return test.frontend.getRenderer()->queryRenderedFeatures(screenCoordinate,
                                                              queryOptions);
}

} // end namespace

TEST(Query, QueryRenderedFeatures) {
    QueryTest test;

    // Batch conversion of latLngs to pixels
    auto points = test.map.pixelsForLatLngs({{0, 0}, {9, 9}});
    ASSERT_EQ(2, points.size());
    // Single conversion of latLng to pixel
    auto point0 = test.map.pixelForLatLng({0, 0});
    ASSERT_NEAR(points[0].x, point0.x, 1e-8);
    ASSERT_NEAR(points[0].y, point0.y, 1e-8);

    auto point1 = test.map.pixelForLatLng({9, 9});
    ASSERT_NEAR(points[1].x, point1.x, 1e-8);
    ASSERT_NEAR(points[1].y, point1.y, 1e-8);

    auto features1 = test.frontend.getRenderer()->queryRenderedFeatures(point0);
    EXPECT_EQ(features1.size(), 4u);

    auto features2 = test.frontend.getRenderer()->queryRenderedFeatures(point1);
    EXPECT_EQ(features2.size(), 0u);
}

TEST(Query, QueryRenderedFeaturesFilterLayer) {
    QueryTest test;

    auto zz = test.map.pixelForLatLng({ 0, 0 });

    auto features1 = test.frontend.getRenderer()->queryRenderedFeatures(zz, {{{ "layer1"}}, {}});
    EXPECT_EQ(features1.size(), 1u);

    auto features2 = test.frontend.getRenderer()->queryRenderedFeatures(zz, {{{ "layer1", "layer2" }}, {}});
    EXPECT_EQ(features2.size(), 2u);

    auto features3 = test.frontend.getRenderer()->queryRenderedFeatures(zz, {{{ "foobar" }}, {}});
    EXPECT_EQ(features3.size(), 0u);

    auto features4 = test.frontend.getRenderer()->queryRenderedFeatures(zz, {{{ "foobar", "layer3" }}, {}});
    EXPECT_EQ(features4.size(), 1u);
}

TEST(Query, QueryRenderedFeaturesFilter) {
    using namespace mbgl::style::expression::dsl;

    QueryTest test;
    auto zz = test.map.pixelForLatLng({ 0, 0 });

    const Filter eqFilter(eq(get("key1"), literal("value1")));
    auto features1 = test.frontend.getRenderer()->queryRenderedFeatures(zz, {{}, { eqFilter }});
    EXPECT_EQ(features1.size(), 1u);

    const Filter idNotEqFilter(ne(id(), literal("feature1")));
    auto features2 = test.frontend.getRenderer()->queryRenderedFeatures(zz, {{{ "layer4" }}, { idNotEqFilter }});
    EXPECT_EQ(features2.size(), 0u);

    const Filter gtFilter(gt(number(get("key2")), literal(1.0)));
    auto features3 = test.frontend.getRenderer()->queryRenderedFeatures(zz, {{ }, { gtFilter }});
    EXPECT_EQ(features3.size(), 1u);
}

TEST(Query, QuerySourceFeatures) {
    QueryTest test;

    auto features1 = test.frontend.getRenderer()->querySourceFeatures("source3");
    EXPECT_EQ(features1.size(), 1u);
}

TEST(Query, QuerySourceFeatureStates) {
    QueryTest test;

    FeatureState newState;
    newState["hover"] = true;
    newState["radius"].set<uint64_t>(20);
    test.frontend.getRenderer()->setFeatureState("source1", {}, "feature1", newState);

    FeatureState states;
    test.frontend.getRenderer()->getFeatureState(states, "source1", {}, "feature1");
    ASSERT_EQ(states.size(), 2u);
    ASSERT_EQ(states["hover"], true);
    ASSERT_EQ(states["radius"].get<uint64_t>(), 20u);
    ASSERT_EQ(newState, states);
}

TEST(Query, QuerySourceFeaturesOptionValidation) {
    QueryTest test;

    // GeoJSONSource, doesn't require a layer id
    auto features = test.frontend.getRenderer()->querySourceFeatures("source3");
    ASSERT_EQ(features.size(), 1u);

    // VectorSource, requires a layer id
    features = test.frontend.getRenderer()->querySourceFeatures("source5");
    ASSERT_EQ(features.size(), 0u);
    
    // RasterSource, not supported
    features = test.frontend.getRenderer()->querySourceFeatures("source6");
    ASSERT_EQ(features.size(), 0u);
}

TEST(Query, QuerySourceFeaturesFilter) {
    using namespace mbgl::style::expression::dsl;

    QueryTest test;

    const Filter eqFilter(eq(get("key1"), literal("value1")));
    auto features1 = test.frontend.getRenderer()->querySourceFeatures("source4", {{}, { eqFilter }});
    EXPECT_EQ(features1.size(), 1u);

    const Filter idNotEqFilter(ne(id(), literal("feature1")));
    auto features2 = test.frontend.getRenderer()->querySourceFeatures("source4", {{}, { idNotEqFilter }});
    EXPECT_EQ(features2.size(), 0u);

    const Filter gtFilter(gt(number(get("key2")), literal(1.0)));
    auto features3 = test.frontend.getRenderer()->querySourceFeatures("source4", {{}, { gtFilter }});
    EXPECT_EQ(features3.size(), 1u);
}

TEST(Query, QueryFeatureExtensionsInvalidExtension) {
    QueryTest test;

    auto unknownExt = test.frontend.getRenderer()->queryFeatureExtensions("source4"s,
                                                                          {},
                                                                          "unknown"s,
                                                                          "children"s);
    auto unknownValue = unknownExt.get<mbgl::Value>();
    EXPECT_TRUE(unknownValue.is<NullValue>());
}

TEST(Query, QueryFeatureExtensionsSuperclusterChildren) {
    QueryTest test;
    auto topClusterFeature = getTopClusterFeature(test);

    EXPECT_EQ(topClusterFeature.size(), 1u);
    const auto featureProps = topClusterFeature[0].properties;
    auto clusterId = featureProps.find("cluster_id"s);
    auto cluster   = featureProps.find("cluster"s);
    EXPECT_TRUE(clusterId != featureProps.end());
    EXPECT_TRUE(cluster != featureProps.end());

    auto queryChildren = test.frontend.getRenderer()->queryFeatureExtensions("cluster_source"s,
                                                                             topClusterFeature[0],
                                                                             "supercluster"s,
                                                                             "children"s);

    EXPECT_TRUE(queryChildren.is<FeatureCollection>());
    auto children = queryChildren.get<FeatureCollection>();
    EXPECT_EQ(children.size(), 4u);

    // Compare results produced by supercluster with default clustering options.
    EXPECT_EQ(children[0].properties["cluster_id"].get<uint64_t>(), 2u);
    EXPECT_EQ(children[1].properties["cluster_id"].get<uint64_t>(), 34u);
    EXPECT_EQ(children[2].properties["cluster_id"].get<uint64_t>(), 258u);
    EXPECT_EQ(children[3].properties["cluster_id"].get<uint64_t>(), 2466u);
    EXPECT_EQ(children[0].properties["point_count"].get<uint64_t>(), 7u);
    EXPECT_EQ(children[1].properties["point_count"].get<uint64_t>(), 16u);
    EXPECT_EQ(children[2].properties["point_count"].get<uint64_t>(), 7u);
    EXPECT_EQ(children[3].properties["point_count"].get<uint64_t>(), 2u);
}

TEST(Query, QueryFeatureExtensionsSuperclusterExpansionZoom) {
    QueryTest test;
    auto topClusterFeature = getTopClusterFeature(test);
    EXPECT_EQ(topClusterFeature.size(), 1u);

    auto queryChildren = test.frontend.getRenderer()->queryFeatureExtensions("cluster_source"s,
                                                                             topClusterFeature[0],
                                                                             "supercluster"s,
                                                                             "children"s);
    auto children = queryChildren.get<FeatureCollection>();


    auto queryExpansionZoom1 = test.frontend.getRenderer()->queryFeatureExtensions("cluster_source"s,
                                                                                    topClusterFeature[0],
                                                                                   "supercluster"s,
                                                                                   "expansion-zoom"s);

    auto queryExpansionZoom2 = test.frontend.getRenderer()->queryFeatureExtensions("cluster_source"s,
                                                                                   children[3],
                                                                                   "supercluster"s,
                                                                                   "expansion-zoom"s);
    auto zoomValue1 = queryExpansionZoom1.get<mbgl::Value>();
    auto zoomValue2 = queryExpansionZoom2.get<mbgl::Value>();
    EXPECT_TRUE(zoomValue1.is<uint64_t>());
    EXPECT_TRUE(zoomValue2.is<uint64_t>());
    EXPECT_EQ(zoomValue1.get<uint64_t>(), 1u);
    EXPECT_EQ(zoomValue2.get<uint64_t>(), 3u);
}

TEST(Query, QueryFeatureExtensionsSuperclusterLeaves) {
    QueryTest test;
    auto topClusterFeature = getTopClusterFeature(test);
    EXPECT_EQ(topClusterFeature.size(), 1u);

    // Get leaves for cluster 1, with default limit 10, offset 0.
    auto queryClusterLeaves = test.frontend.getRenderer()->queryFeatureExtensions("cluster_source"s,
                                                                                  topClusterFeature[0],
                                                                                  "supercluster"s,
                                                                                  "leaves"s);
    EXPECT_TRUE(queryClusterLeaves.is<FeatureCollection>());
    auto leaves = queryClusterLeaves.get<FeatureCollection>();
    EXPECT_EQ(leaves.size(), 10u);

    // Get leaves for cluster 1, with limit 3, offset 0.
    const std::map<std::string, mbgl::Value> limitOpts = { {"limit"s, static_cast<uint64_t>(3u)} };
    auto queryClusterLeavesLimit3 = test.frontend.getRenderer()->queryFeatureExtensions("cluster_source"s,
                                                                                        topClusterFeature[0],
                                                                                        "supercluster"s,
                                                                                        "leaves"s,
                                                                                        limitOpts);
    auto limitLeaves3 = queryClusterLeavesLimit3.get<FeatureCollection>();
    EXPECT_EQ(limitLeaves3.size(), 3u);

    EXPECT_EQ(limitLeaves3[0].properties["name"].get<std::string>(), "Niagara Falls"s);
    EXPECT_EQ(limitLeaves3[1].properties["name"].get<std::string>(), "Cape May"s);
    EXPECT_EQ(limitLeaves3[2].properties["name"].get<std::string>(), "Cape Fear"s);

    // Get leaves for cluster 1, with limit 3, offset 3.
    const std::map<std::string, mbgl::Value> offsetOpts = { {"limit"s, static_cast<uint64_t>(3u)},
                                                            {"offset"s, static_cast<uint64_t>(3u)} };
    auto queryClusterLeavesOffset3 = test.frontend.getRenderer()->queryFeatureExtensions("cluster_source"s,
                                                                                        topClusterFeature[0],
                                                                                        "supercluster"s,
                                                                                        "leaves"s,
                                                                                        offsetOpts);
    auto offsetLeaves3 = queryClusterLeavesOffset3.get<FeatureCollection>();
    EXPECT_EQ(offsetLeaves3.size(), 3u);
    EXPECT_EQ(offsetLeaves3[0].properties["name"].get<std::string>(), "Cape Hatteras"s);
    EXPECT_EQ(offsetLeaves3[1].properties["name"].get<std::string>(), "Cape Sable"s);
    EXPECT_EQ(offsetLeaves3[2].properties["name"].get<std::string>(), "Cape Cod"s);
}
