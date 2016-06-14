#include <mbgl/test/util.hpp>
#include <mbgl/tile/geometry_tile.hpp>

using namespace mbgl;

TEST(GeometryTile, classifyRings1) {
    std::vector<GeometryCollection> polygons = classifyRings({
      { {0, 0}, {0, 40}, {40, 40}, {40, 0}, {0, 0} }
    });

    // output: 1 polygon
    ASSERT_EQ(polygons.size(), 1);
    // output: polygon 1 has 1 exterior
    ASSERT_EQ(polygons[0].size(), 1);
}

TEST(GeometryTile, classifyRings2) {
    std::vector<GeometryCollection> polygons = classifyRings({
      { {0, 0}, {0, 40}, {40, 40}, {40, 0}, {0, 0} },
      { {10, 10}, {20, 10}, {20, 20}, {10, 10} }
    });

    // output: 1 polygon
    ASSERT_EQ(polygons.size(), 1);
    // output: polygon 1 has 1 exterior, 1 interior
    ASSERT_EQ(polygons[0].size(), 2);
}

TEST(GeometryTile, limitHoles1) {
    GeometryCollection polygon = {
      { {0, 0}, {0, 40}, {40, 40}, {40, 0}, {0, 0} },
      { {30, 30}, {32, 30}, {32, 32}, {30, 30} },
      { {10, 10}, {20, 10}, {20, 20}, {10, 10} }
    };

    limitHoles(polygon, 1);

    // output: polygon 1 has 1 exterior, 1 interior
    ASSERT_EQ(polygon.size(), 2);

    // ensure we've kept the right rings (ones with largest areas)
    ASSERT_EQ(polygon[0][0].x, 0);
    ASSERT_EQ(polygon[1][0].x, 10);
}

TEST(GeometryTile, limitHoles2) {
    GeometryCollection polygon = {
      { {0, 0}, {0, 40}, {40, 40}, {40, 0}, {0, 0} },
      { {10, 10}, {20, 10}, {20, 20}, {10, 10} },
      { {30, 30}, {32, 30}, {32, 32}, {30, 30} }
    };

    limitHoles(polygon, 1);

    // output: polygon 1 has 1 exterior, 1 interior
    ASSERT_EQ(polygon.size(), 2);

    // ensure we've kept the right rings (ones with largest areas)
    ASSERT_EQ(polygon[0][0].x, 0);
    ASSERT_EQ(polygon[1][0].x, 10);
}
