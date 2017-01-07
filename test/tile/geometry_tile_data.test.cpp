#include <mbgl/test/util.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

using namespace mbgl;

static double _signedArea(const GeometryCoordinates& ring) {
    double sum = 0;

    for (std::size_t i = 0, len = ring.size(), j = len - 1; i < len; j = i++) {
        const GeometryCoordinate& p1 = ring[i];
        const GeometryCoordinate& p2 = ring[j];
        sum += (p2.x - p1.x) * (p1.y + p2.y);
    }

    return sum;
}

TEST(GeometryTileData, classifyRings1) {
    std::vector<GeometryCollection> polygons = classifyRings({
      { {0, 0}, {0, 40}, {40, 40}, {40, 0}, {0, 0} }
    });

    // output: 1 polygon
    ASSERT_EQ(polygons.size(), 1u);
    // output: polygon 1 has 1 exterior
    ASSERT_EQ(polygons[0].size(), 1u);
}

TEST(GeometryTileData, classifyRings2) {
    std::vector<GeometryCollection> polygons = classifyRings({
      { {0, 0}, {0, 40}, {40, 40}, {40, 0}, {0, 0} },
      { {10, 10}, {20, 10}, {20, 20}, {10, 10} }
    });

    // output: 1 polygon
    ASSERT_EQ(polygons.size(), 1u);
    // output: polygon 1 has 1 exterior, 1 interior
    ASSERT_EQ(polygons[0].size(), 2u);
}

TEST(GeometryTileData, limitHoles1) {
    GeometryCollection polygon = {
      { {0, 0}, {0, 40}, {40, 40}, {40, 0}, {0, 0} },
      { {30, 30}, {32, 30}, {32, 32}, {30, 30} },
      { {10, 10}, {20, 10}, {20, 20}, {10, 10} }
    };

    limitHoles(polygon, 1);

    // output: polygon 1 has 1 exterior, 1 interior
    ASSERT_EQ(polygon.size(), 2u);

    // ensure we've kept the right rings (ones with largest areas)
    ASSERT_EQ(polygon[0][0].x, 0);
    ASSERT_EQ(polygon[1][0].x, 10);
}

TEST(GeometryTileData, limitHoles2) {
    GeometryCollection polygon = {
      { {0, 0}, {0, 40}, {40, 40}, {40, 0}, {0, 0} },
      { {10, 10}, {20, 10}, {20, 20}, {10, 10} },
      { {30, 30}, {32, 30}, {32, 32}, {30, 30} }
    };

    limitHoles(polygon, 1);

    // output: polygon 1 has 1 exterior, 1 interior
    ASSERT_EQ(polygon.size(), 2u);

    // ensure we've kept the right rings (ones with largest areas)
    ASSERT_EQ(polygon[0][0].x, 0);
    ASSERT_EQ(polygon[1][0].x, 10);
}

TEST(GeometryTileData, limitHoles3) {
    // real world polygon with interior rings with negative areas
    // that need to be sorted in `limitHoles` by comparing absolute
    // area not signed
    GeometryCollection polygon = {
     { {7336,-248},{7304,-248},{7272,-168},{7176,-200},{7080,-136},{7048,-56},{7128,-8},{7176,-56},{7288,-56},{7316,0},{6918,0},{6904,-40},{6984,-72},{6952,-88},{6952,-168},{6888,-88},{6856,-88},{6856,-8},{6872,0},{6170,0},{6184,-40},{6136,-72},{6104,-56},{6132,0},{6028,0},{6104,-152},{6184,-200},{6206,-256},{6272,-256},{6264,-248},{6248,-120},{6280,-136},{6280,-232},{6288,-256},{6790,-256},{6792,-248},{6800,-256},{7058,-256},{7064,-248},{7096,-256},{7338,-256},{7336,-248} },
     { {6344,-104},{6264,-8},{6392,-72},{6360,-200},{6344,-104} },
     { {6744,-24},{6760,-72},{6728,-104},{6744,-24} },
     { {6616,-104},{6648,-88},{6632,-72},{6664,-56},{6664,-120},{6616,-104} }
    };

    // make a copy for later testing
    GeometryCollection original(polygon);

    ASSERT_EQ(polygon.size(), 4u);
    ASSERT_EQ(_signedArea(polygon.at(0)), 515360); // exterior
    ASSERT_EQ(_signedArea(polygon.at(1)), -12288); // biggest interior ring
    ASSERT_EQ(_signedArea(polygon.at(2)), -2048);  // smallest interior ring
    ASSERT_EQ(_signedArea(polygon.at(3)), -3072);  // second largest interior ring

    limitHoles(polygon, 2);

    // output: polygon 1 has 1 exterior, 2 interior
    ASSERT_EQ(polygon.size(), 3u);

    // ensure we've kept the exterior ring
    ASSERT_EQ(original.at(0), polygon.at(0));

    // ensure we've kept the two largest interior rings
    ASSERT_EQ(original.at(1), polygon.at(1));
    ASSERT_EQ(original.at(3), polygon.at(2));

}
