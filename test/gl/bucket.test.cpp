#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_geometry_tile_feature.hpp>

#include <mbgl/renderer/buckets/circle_bucket.hpp>
#include <mbgl/renderer/buckets/fill_bucket.hpp>
#include <mbgl/renderer/buckets/line_bucket.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/gl/context.hpp>

#include <mbgl/map/mode.hpp>

using namespace mbgl;

TEST(Buckets, CircleBucket) {
    gl::Context context;
    CircleBucket bucket { { {0, 0, 0}, MapMode::Still, 1.0 }, {} };
    ASSERT_FALSE(bucket.hasData());
    ASSERT_FALSE(bucket.needsUpload());

    GeometryCollection point { { { 0, 0 } } };
    bucket.addFeature(StubGeometryTileFeature { {}, FeatureType::Point, point, {} }, point);
    ASSERT_TRUE(bucket.hasData());
    ASSERT_TRUE(bucket.needsUpload());

    bucket.upload(context);
    ASSERT_TRUE(bucket.hasData());
    ASSERT_FALSE(bucket.needsUpload());
}

TEST(Buckets, FillBucket) {
    gl::Context context;
    FillBucket bucket { { {0, 0, 0}, MapMode::Still, 1.0 }, {} };
    ASSERT_FALSE(bucket.hasData());
    ASSERT_FALSE(bucket.needsUpload());

    GeometryCollection polygon { { { 0, 0 }, { 0, 1 }, { 1, 1 } } };
    bucket.addFeature(StubGeometryTileFeature { {}, FeatureType::Polygon, polygon, {} }, polygon);
    ASSERT_TRUE(bucket.hasData());
    ASSERT_TRUE(bucket.needsUpload());

    bucket.upload(context);
    ASSERT_FALSE(bucket.needsUpload());
}

TEST(Buckets, LineBucket) {
    gl::Context context;
    LineBucket bucket { { {0, 0, 0}, MapMode::Still, 1.0 }, {}, {} };
    ASSERT_FALSE(bucket.hasData());
    ASSERT_FALSE(bucket.needsUpload());

    // Ignore invalid feature type.
    GeometryCollection point { { { 0, 0 } } };
    bucket.addFeature(StubGeometryTileFeature { {}, FeatureType::Point, point, {} }, point);
    ASSERT_FALSE(bucket.hasData());

    GeometryCollection line { { { 0, 0 }, { 1, 1 } } };
    bucket.addFeature(StubGeometryTileFeature { {}, FeatureType::LineString, line, {} }, line);
    ASSERT_TRUE(bucket.hasData());
    ASSERT_TRUE(bucket.needsUpload());

    bucket.upload(context);
    ASSERT_FALSE(bucket.needsUpload());
}

TEST(Buckets, SymbolBucket) {
    style::SymbolLayoutProperties::PossiblyEvaluated layout;
    bool sdfIcons = false;
    bool iconsNeedLinear = false;

    gl::Context context;
    SymbolBucket bucket { layout, {}, 16.0f, 1.0f, 0, sdfIcons, iconsNeedLinear };
    ASSERT_FALSE(bucket.hasIconData());
    ASSERT_FALSE(bucket.hasTextData());
    ASSERT_FALSE(bucket.hasCollisionBoxData());
    ASSERT_FALSE(bucket.hasData());
    ASSERT_FALSE(bucket.needsUpload());

    // SymbolBucket::addFeature() is a no-op.
    GeometryCollection point { { { 0, 0 } } };
    bucket.addFeature(StubGeometryTileFeature { {}, FeatureType::Point, point, {} }, point);
    ASSERT_FALSE(bucket.hasData());
    ASSERT_FALSE(bucket.needsUpload());

    bucket.text.segments.emplace_back(gl::SegmentInfo{ 0, 0 });
    ASSERT_TRUE(bucket.hasTextData());
    ASSERT_TRUE(bucket.hasData());
    ASSERT_TRUE(bucket.needsUpload());

    bucket.upload(context);
    ASSERT_FALSE(bucket.needsUpload());
}

TEST(Buckets, RasterBucket) {
    gl::Context context;
    UnassociatedImage rgba({ 1, 1 });

    // RasterBucket::hasData() is always true.
    RasterBucket bucket = { std::move(rgba) };
    ASSERT_TRUE(bucket.hasData());
    ASSERT_TRUE(bucket.needsUpload());

    bucket.upload(context);
    ASSERT_FALSE(bucket.needsUpload());

    bucket.clear();
    ASSERT_TRUE(bucket.needsUpload());
}
