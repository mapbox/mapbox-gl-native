#include <mbgl/test/util.hpp>
#include <mbgl/tile/geometry_tile.hpp>

using namespace mbgl;

TEST(classifyRings, Unbounded1) {
    GeometryCollection unclassified;
    GeometryCoordinates ring1;

    ring1.push_back(Point<short int>(0, 0));
    ring1.push_back(Point<short int>(0, 40));
    ring1.push_back(Point<short int>(40, 40));
    ring1.push_back(Point<short int>(40, 0));
    ring1.push_back(Point<short int>(0, 0));
    unclassified.push_back(ring1);

    std::vector<GeometryCollection> classified = classifyRings(unclassified, 0);
    // input: 1 unclassified ring
    ASSERT_EQ(unclassified.size(), 1);
    // output: 1 polygon
    ASSERT_EQ(classified.size(), 1);
    // output: polygon 1 has 1 exterior
    ASSERT_EQ(classified[0].size(), 1);
}

TEST(classifyRings, Unbounded2) {
    GeometryCollection unclassified;
    GeometryCoordinates ring1;
    GeometryCoordinates ring2;

    ring1.push_back(Point<short int>(0, 0));
    ring1.push_back(Point<short int>(0, 40));
    ring1.push_back(Point<short int>(40, 40));
    ring1.push_back(Point<short int>(40, 0));
    ring1.push_back(Point<short int>(0, 0));
    unclassified.push_back(ring1);

    ring2.push_back(Point<short int>(10, 10));
    ring2.push_back(Point<short int>(20, 10));
    ring2.push_back(Point<short int>(20, 20));
    ring2.push_back(Point<short int>(10, 10));
    unclassified.push_back(ring2);

    std::vector<GeometryCollection> classified = classifyRings(unclassified, 0);
    // input: 2 unclassified rings
    ASSERT_EQ(unclassified.size(), 2);
    // output: 1 polygon
    ASSERT_EQ(classified.size(), 1);
    // output: polygon 1 has 1 exterior, 1 interior
    ASSERT_EQ(classified[0].size(), 2);
}

TEST(classifyRings, maxRings1) {
    GeometryCollection unclassified;
    GeometryCoordinates ring1;
    GeometryCoordinates ring2;
    GeometryCoordinates ring3;

    ring1.push_back(Point<short int>(0, 0));
    ring1.push_back(Point<short int>(0, 40));
    ring1.push_back(Point<short int>(40, 40));
    ring1.push_back(Point<short int>(40, 0));
    ring1.push_back(Point<short int>(0, 0));
    unclassified.push_back(ring1);

    ring2.push_back(Point<short int>(30, 30));
    ring2.push_back(Point<short int>(32, 30));
    ring2.push_back(Point<short int>(32, 32));
    ring2.push_back(Point<short int>(30, 30));
    unclassified.push_back(ring2);

    ring3.push_back(Point<short int>(10, 10));
    ring3.push_back(Point<short int>(20, 10));
    ring3.push_back(Point<short int>(20, 20));
    ring3.push_back(Point<short int>(10, 10));
    unclassified.push_back(ring3);

    std::vector<GeometryCollection> classified = classifyRings(unclassified, 0);
    // input: 3 unclassified rings
    ASSERT_EQ(unclassified.size(), 3);
    // output: 1 polygon
    ASSERT_EQ(classified.size(), 1);
    // output: polygon 1 has 1 exterior, 2 interior
    ASSERT_EQ(classified[0].size(), 3);

    std::vector<GeometryCollection> truncated = classifyRings(unclassified, 2);
    // output: 1 polygon
    ASSERT_EQ(truncated.size(), 1);
    // output: polygon 1 has 1 exterior, 1 interior
    ASSERT_EQ(truncated[0].size(), 2);
    // ensure we've kept the right rings (ones with largest areas)
    ASSERT_EQ(truncated[0][0][0].x, 0);
    ASSERT_EQ(truncated[0][1][0].x, 10);
}

TEST(classifyRings, maxRingsReversed) {
    GeometryCollection unclassified;
    GeometryCoordinates ring1;
    GeometryCoordinates ring2;
    GeometryCoordinates ring3;

    ring1.push_back(Point<short int>(0, 0));
    ring1.push_back(Point<short int>(40, 0));
    ring1.push_back(Point<short int>(40, 40));
    ring1.push_back(Point<short int>(0, 40));
    ring1.push_back(Point<short int>(0, 0));
    unclassified.push_back(ring1);

    ring2.push_back(Point<short int>(30, 30));
    ring2.push_back(Point<short int>(32, 32));
    ring2.push_back(Point<short int>(32, 30));
    ring2.push_back(Point<short int>(30, 30));
    unclassified.push_back(ring2);

    ring3.push_back(Point<short int>(10, 10));
    ring3.push_back(Point<short int>(20, 20));
    ring3.push_back(Point<short int>(20, 10));
    ring3.push_back(Point<short int>(10, 10));
    unclassified.push_back(ring3);

    std::vector<GeometryCollection> truncated = classifyRings(unclassified, 2);
    // output: 1 polygon
    ASSERT_EQ(truncated.size(), 1);
    // output: polygon 1 has 1 exterior, 1 interior
    ASSERT_EQ(truncated[0].size(), 2);
    // ensure we've kept the right rings (ones with largest areas)
    ASSERT_EQ(truncated[0][0][0].x, 0);
    ASSERT_EQ(truncated[0][1][0].x, 10);
}

