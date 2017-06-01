#include <mbgl/test/util.hpp>

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
    CircleBucket bucket { { {0, 0, 0}, MapMode::Still, 1.0 }, {} };
    ASSERT_FALSE(bucket.hasData());
}

TEST(Buckets, FillBucket) {
    FillBucket bucket { { {0, 0, 0}, MapMode::Still, 1.0 }, {} };
    ASSERT_FALSE(bucket.hasData());
}

TEST(Buckets, LineBucket) {
    LineBucket bucket { { {0, 0, 0}, MapMode::Still, 1.0 }, {}, {} };
    ASSERT_FALSE(bucket.hasData());
}

TEST(Buckets, SymbolBucket) {
    style::SymbolLayoutProperties::PossiblyEvaluated layout;
    bool sdfIcons = false;
    bool iconsNeedLinear = false;

    SymbolBucket bucket { layout, {}, 16.0f, 1.0f, 0, sdfIcons, iconsNeedLinear };
    ASSERT_FALSE(bucket.hasIconData());
    ASSERT_FALSE(bucket.hasTextData());
    ASSERT_FALSE(bucket.hasCollisionBoxData());
}

TEST(Buckets, RasterBucket) {
    gl::Context context;
    UnassociatedImage rgba({ 1, 1 });

    RasterBucket bucket = { std::move(rgba) };
    ASSERT_TRUE(bucket.needsUpload());

    bucket.upload(context);
    ASSERT_FALSE(bucket.needsUpload());

    bucket.clear();
    ASSERT_TRUE(bucket.needsUpload());
}
