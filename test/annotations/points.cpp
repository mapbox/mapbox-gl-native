#include "../fixtures/util.hpp"

#include <mbgl/annotation/point_annotation_layer.hpp>

#include <set>
#include <cstdlib>

using namespace mbgl;

TEST(Annotations, Points) {
    PointAnnotationLayer layer;

    layer.add(std::make_shared<const Annotation>(1, AnnotationType::Point, "default_marker", LatLng{ 13, 52 }));

    {
        const auto result = layer.getInBounds(TileID{ 0, 0, 0, 1 });
        EXPECT_EQ(1u, result.size());
        EXPECT_DOUBLE_EQ(13, result[0]->point.latitude);
        EXPECT_DOUBLE_EQ(52, result[0]->point.longitude);
    }
    {
        const auto result = layer.getInBounds(TileID{ 1, 0, 0, 1 });
        EXPECT_EQ(0u, result.size());
    }
    {
        const auto result = layer.getInBounds(TileID{ 1, 1, 0, 1 });
        EXPECT_EQ(1u, result.size());
        EXPECT_DOUBLE_EQ(13, result[0]->point.latitude);
        EXPECT_DOUBLE_EQ(52, result[0]->point.longitude);
    }
    {
        const auto result = layer.getInBounds(TileID{ 1, 0, 1, 1 });
        EXPECT_EQ(0u, result.size());
    }
    {
        const auto result = layer.getInBounds(TileID{ 1, 1, 1, 1 });
        EXPECT_EQ(0u, result.size());
    }
}


TEST(Annotations, Points2) {
    std::srand(std::time(0));

    constexpr size_t total = 1000;

    std::set<std::shared_ptr<const Annotation>> annotations;
    const LatLng sw { -85, -180 };
    const LatLng ne { 85, 180 };
    for (size_t i = 0; i < total; i++) {
        const double lon = sw.longitude + (ne.longitude - sw.longitude) * (double(std::rand()) / RAND_MAX);
        const double lat = sw.latitude + (ne.latitude - sw.latitude) * (double(std::rand()) / RAND_MAX);
        annotations.emplace(std::make_shared<const Annotation>(i, AnnotationType::Point, "default_marker", LatLng{ lat, lon }));
    }

    PointAnnotationLayer layer;
    layer.add({ annotations.begin(), annotations.end() });

    {
        const auto result = layer.getInBounds(TileID{ 0, 0, 0, 0 });
        EXPECT_EQ(total, result.size());
    }

    const auto result_100 = layer.getInBounds(TileID{ 1, 0, 0, 1 });
    const auto result_101 = layer.getInBounds(TileID{ 1, 0, 1, 1 });
    const auto result_110 = layer.getInBounds(TileID{ 1, 1, 0, 1 });
    const auto result_111 = layer.getInBounds(TileID{ 1, 1, 1, 1 });

    EXPECT_EQ(total, result_100.size() + result_101.size() + result_110.size() + result_111.size());

    std::set<std::shared_ptr<const Annotation>> result;
    result.insert(result_100.begin(), result_100.end());
    result.insert(result_101.begin(), result_101.end());
    result.insert(result_110.begin(), result_110.end());
    result.insert(result_111.begin(), result_111.end());
    EXPECT_EQ(total, result.size());
}


TEST(Annotations, Points3) {
    PointAnnotationLayer layer;

    layer.add(std::make_shared<const Annotation>(1, AnnotationType::Point, "default_marker", LatLng{ 52.520803, 13.40945 }));

    {
        const auto result = layer.getInBounds(TileID{ 0, 0, 0, 1 });
        EXPECT_EQ(1u, result.size());
    }

    for (int z = 18, x = 140836, y = 85970; z > 0 ; --z, x /= 2, y /= 2) {
        const auto result = layer.getInBounds(TileID{ int8_t(z), x, y, int8_t(z) });
        EXPECT_EQ(1u, result.size());
    }
}
