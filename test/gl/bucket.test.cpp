#include <mbgl/test/util.hpp>

#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>

#include <mbgl/style/layers/symbol_layer_properties.hpp>

#include <mbgl/map/mode.hpp>

TEST(Buckets, CircleBucket) {
    mbgl::MapMode mapMode = mbgl::MapMode::Still;

    mbgl::CircleBucket bucket { mapMode };
    ASSERT_FALSE(bucket.hasData());
}

TEST(Buckets, FillBucket) {
    mbgl::FillBucket bucket;
    ASSERT_FALSE(bucket.hasData());
}

TEST(Buckets, LineBucket) {
    uint32_t overscaling = 0;

    mbgl::LineBucket bucket { overscaling };
    ASSERT_FALSE(bucket.hasData());
}

TEST(Buckets, SymbolBucket) {
    mbgl::MapMode mapMode = mbgl::MapMode::Still;
    mbgl::style::SymbolLayoutProperties::Evaluated properties;
    bool sdfIcons = false;
    bool iconsNeedLinear = false;

    mbgl::SymbolBucket bucket { mapMode, properties, sdfIcons, iconsNeedLinear };
    ASSERT_FALSE(bucket.hasIconData());
    ASSERT_FALSE(bucket.hasTextData());
    ASSERT_FALSE(bucket.hasCollisionBoxData());
}
