#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_geometry_tile_feature.hpp>

#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/renderer/buckets/circle_bucket.hpp>
#include <mbgl/renderer/buckets/fill_bucket.hpp>
#include <mbgl/renderer/buckets/line_bucket.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/headless_backend.hpp>

#include <mbgl/map/mode.hpp>

namespace mbgl {

template <class Attributes>
bool operator==(const Segment<Attributes>& lhs, const Segment<Attributes>& rhs) {
    return std::tie(lhs.vertexOffset, lhs.indexOffset, lhs.vertexLength, lhs.indexLength) ==
           std::tie(rhs.vertexOffset, rhs.indexOffset, rhs.vertexLength, rhs.indexLength);
}

namespace gl {
namespace detail {

template <class A1, class A2>
bool operator==(const Vertex<A1, A2>& lhs, const Vertex<A1, A2>& rhs) {
    return std::tie(lhs.a1, lhs.a2) == std::tie(rhs.a1, rhs.a2);
}

} // namespace detail
} // namespace gl
} // namespace mbgl

using namespace mbgl;

namespace {

PropertyMap properties;

} // namespace

TEST(Buckets, CircleBucket) {
    HeadlessBackend backend({ 512, 256 });
    BackendScope scope { backend };

    gl::Context context;
    CircleBucket bucket { { {0, 0, 0}, MapMode::Static, 1.0 }, {} };
    ASSERT_FALSE(bucket.hasData());
    ASSERT_FALSE(bucket.needsUpload());

    GeometryCollection point { { { 0, 0 } } };
    bucket.addFeature(StubGeometryTileFeature { {}, FeatureType::Point, point, properties }, point);
    ASSERT_TRUE(bucket.hasData());
    ASSERT_TRUE(bucket.needsUpload());

    bucket.upload(context);
    ASSERT_TRUE(bucket.hasData());
    ASSERT_FALSE(bucket.needsUpload());
}

TEST(Buckets, FillBucket) {
    HeadlessBackend backend({ 512, 256 });
    BackendScope scope { backend };

    gl::Context context;
    FillBucket bucket { { {0, 0, 0}, MapMode::Static, 1.0 }, {} };
    ASSERT_FALSE(bucket.hasData());
    ASSERT_FALSE(bucket.needsUpload());

    GeometryCollection polygon { { { 0, 0 }, { 0, 1 }, { 1, 1 } } };
    bucket.addFeature(StubGeometryTileFeature { {}, FeatureType::Polygon, polygon, properties }, polygon);
    ASSERT_TRUE(bucket.hasData());
    ASSERT_TRUE(bucket.needsUpload());

    bucket.upload(context);
    ASSERT_FALSE(bucket.needsUpload());
}

TEST(Buckets, LineBucket) {
    HeadlessBackend backend({ 512, 256 });
    BackendScope scope { backend };

    gl::Context context;
    LineBucket bucket { { {0, 0, 0}, MapMode::Static, 1.0 }, {}, {} };
    ASSERT_FALSE(bucket.hasData());
    ASSERT_FALSE(bucket.needsUpload());

    // Ignore invalid feature type.
    GeometryCollection point { { { 0, 0 } } };
    bucket.addFeature(StubGeometryTileFeature { {}, FeatureType::Point, point, properties }, point);
    ASSERT_FALSE(bucket.hasData());

    GeometryCollection line { { { 0, 0 }, { 1, 1 } } };
    bucket.addFeature(StubGeometryTileFeature { {}, FeatureType::LineString, line, properties }, line);
    ASSERT_TRUE(bucket.hasData());
    ASSERT_TRUE(bucket.needsUpload());

    bucket.upload(context);
    ASSERT_FALSE(bucket.needsUpload());
}

TEST(Buckets, SymbolBucket) {
    HeadlessBackend backend({ 512, 256 });
    BackendScope scope { backend };

    style::SymbolLayoutProperties::PossiblyEvaluated layout;
    bool sdfIcons = false;
    bool iconsNeedLinear = false;
    bool sortFeaturesByY = false;
    std::vector<SymbolInstance> symbolInstances;

    gl::Context context;
    SymbolBucket bucket { layout, {}, 16.0f, 1.0f, 0, sdfIcons, iconsNeedLinear, sortFeaturesByY, std::move(symbolInstances) };
    ASSERT_FALSE(bucket.hasIconData());
    ASSERT_FALSE(bucket.hasTextData());
    ASSERT_FALSE(bucket.hasCollisionBoxData());
    ASSERT_FALSE(bucket.hasData());
    ASSERT_FALSE(bucket.needsUpload());

    // SymbolBucket::addFeature() is a no-op.
    GeometryCollection point { { { 0, 0 } } };
    bucket.addFeature(StubGeometryTileFeature { {}, FeatureType::Point, point, properties }, point);
    ASSERT_FALSE(bucket.hasData());
    ASSERT_FALSE(bucket.needsUpload());

    bucket.text.segments.emplace_back(0, 0);
    ASSERT_TRUE(bucket.hasTextData());
    ASSERT_TRUE(bucket.hasData());
    ASSERT_TRUE(bucket.needsUpload());

    bucket.upload(context);
    ASSERT_FALSE(bucket.needsUpload());
}

TEST(Buckets, RasterBucket) {
    HeadlessBackend backend({ 512, 256 });
    BackendScope scope { backend };

    gl::Context context;
    PremultipliedImage rgba({ 1, 1 });

    // RasterBucket::hasData() is always true.
    RasterBucket bucket = { std::move(rgba) };
    ASSERT_TRUE(bucket.hasData());
    ASSERT_TRUE(bucket.needsUpload());

    bucket.upload(context);
    ASSERT_FALSE(bucket.needsUpload());

    bucket.clear();
    ASSERT_TRUE(bucket.needsUpload());
}

TEST(Buckets, RasterBucketMaskEmpty) {
    RasterBucket bucket{ nullptr };
    bucket.setMask({});
    EXPECT_EQ((std::vector<RasterLayoutVertex>{}), bucket.vertices.vector());
    EXPECT_EQ((std::vector<uint16_t>{}), bucket.indices.vector());
    SegmentVector<RasterAttributes> expectedSegments;
    expectedSegments.emplace_back(0, 0, 0, 0);
    EXPECT_EQ(expectedSegments, bucket.segments);
}

TEST(Buckets, RasterBucketMaskNoChildren) {
    RasterBucket bucket{ nullptr };
    bucket.setMask({ CanonicalTileID{ 0, 0, 0 } });

    // A mask of 0/0/0 doesn't produce buffers since we're instead using the global shared buffers.
    EXPECT_EQ((std::vector<RasterLayoutVertex>{}), bucket.vertices.vector());
    EXPECT_EQ((std::vector<uint16_t>{}), bucket.indices.vector());
    EXPECT_EQ((SegmentVector<RasterAttributes>{}), bucket.segments);
}

 TEST(Buckets, RasterBucketMaskTwoChildren) {
     RasterBucket bucket{ nullptr };
     bucket.setMask(
         { CanonicalTileID{ 1, 0, 0 }, CanonicalTileID{ 1, 1, 1 } });

     EXPECT_EQ(
         (std::vector<RasterLayoutVertex>{
             // 1/0/1
             RasterProgram::layoutVertex({ 0, 0 }, { 0, 0 }),
             RasterProgram::layoutVertex({ 4096, 0 }, { 4096, 0 }),
             RasterProgram::layoutVertex({ 0, 4096 }, { 0, 4096 }),
             RasterProgram::layoutVertex({ 4096, 4096 }, { 4096, 4096 }),

             // 1/1/1
             RasterProgram::layoutVertex({ 4096, 4096 }, { 4096, 4096 }),
             RasterProgram::layoutVertex({ 8192, 4096 }, { 8192, 4096 }),
             RasterProgram::layoutVertex({ 4096, 8192 }, { 4096, 8192 }),
             RasterProgram::layoutVertex({ 8192, 8192 }, { 8192, 8192 }),
         }),
         bucket.vertices.vector());

     EXPECT_EQ(
         (std::vector<uint16_t>{
             // 1/0/1
             0, 1, 2,
             1, 2, 3,

             // 1/1/1
             4, 5, 6,
             5, 6, 7,
         }),
         bucket.indices.vector());


     SegmentVector<RasterAttributes> expectedSegments;
     expectedSegments.emplace_back(0, 0, 8, 12);
     EXPECT_EQ(expectedSegments, bucket.segments);
 }

 TEST(Buckets, RasterBucketMaskComplex) {
     RasterBucket bucket{ nullptr };
     bucket.setMask(
         { CanonicalTileID{ 1, 0, 1 }, CanonicalTileID{ 1, 1, 0 }, CanonicalTileID{ 2, 2, 3 },
           CanonicalTileID{ 2, 3, 2 }, CanonicalTileID{ 3, 6, 7 }, CanonicalTileID{ 3, 7, 6 } });

     EXPECT_EQ(
         (std::vector<RasterLayoutVertex>{
             // 1/0/1
             RasterProgram::layoutVertex({ 0, 4096 }, { 0, 4096 }),
             RasterProgram::layoutVertex({ 4096, 4096 }, { 4096, 4096 }),
             RasterProgram::layoutVertex({ 0, 8192 }, { 0, 8192 }),
             RasterProgram::layoutVertex({ 4096, 8192 }, { 4096, 8192 }),

             // 1/1/0
             RasterProgram::layoutVertex({ 4096, 0 }, { 4096, 0 }),
             RasterProgram::layoutVertex({ 8192, 0 }, { 8192, 0 }),
             RasterProgram::layoutVertex({ 4096, 4096 }, { 4096, 4096 }),
             RasterProgram::layoutVertex({ 8192, 4096 }, { 8192, 4096 }),

             // 2/2/3
             RasterProgram::layoutVertex({ 4096, 6144 }, { 4096, 6144 }),
             RasterProgram::layoutVertex({ 6144, 6144 }, { 6144, 6144 }),
             RasterProgram::layoutVertex({ 4096, 8192 }, { 4096, 8192 }),
             RasterProgram::layoutVertex({ 6144, 8192 }, { 6144, 8192 }),

             // 2/3/2
             RasterProgram::layoutVertex({ 6144, 4096 }, { 6144, 4096 }),
             RasterProgram::layoutVertex({ 8192, 4096 }, { 8192, 4096 }),
             RasterProgram::layoutVertex({ 6144, 6144 }, { 6144, 6144 }),
             RasterProgram::layoutVertex({ 8192, 6144 }, { 8192, 6144 }),

             // 3/6/7
             RasterProgram::layoutVertex({ 6144, 7168 }, { 6144, 7168 }),
             RasterProgram::layoutVertex({ 7168, 7168 }, { 7168, 7168 }),
             RasterProgram::layoutVertex({ 6144, 8192 }, { 6144, 8192 }),
             RasterProgram::layoutVertex({ 7168, 8192 }, { 7168, 8192 }),

             // 3/7/6
             RasterProgram::layoutVertex({ 7168, 6144 }, { 7168, 6144 }),
             RasterProgram::layoutVertex({ 8192, 6144 }, { 8192, 6144 }),
             RasterProgram::layoutVertex({ 7168, 7168 }, { 7168, 7168 }),
             RasterProgram::layoutVertex({ 8192, 7168 }, { 8192, 7168 }),
         }),
         bucket.vertices.vector());

     EXPECT_EQ(
         (std::vector<uint16_t>{
             // 1/0/1
             0, 1, 2,
             1, 2, 3,

             // 1/1/0
             4, 5, 6,
             5, 6, 7,

             // 2/2/3
             8, 9, 10,
             9, 10, 11,

             // 2/3/2
             12, 13, 14,
             13, 14, 15,

             // 3/6/7
             16, 17, 18,
             17, 18, 19,

             // 3/7/6
             20, 21, 22,
             21, 22, 23,
         }),
         bucket.indices.vector());


     SegmentVector<RasterAttributes> expectedSegments;
     expectedSegments.emplace_back(0, 0, 24, 36);
     EXPECT_EQ(expectedSegments, bucket.segments);
 }
