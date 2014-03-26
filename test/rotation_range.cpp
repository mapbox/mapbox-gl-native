#include <iostream>
#include "gtest/gtest.h"

#include <llmr/text/rotation_range.hpp>

using namespace llmr;

double deg(double x) { return x / M_PI * 180.0; }

TEST(RotationRange, mergeCollisions) {
    // merge overlapping ranges
    EXPECT_EQ(
        CollisionRange({{1.0 / 8.0 * M_PI, 6.0 / 8.0 * M_PI}}),
        mergeCollisions(
            CollisionList(
                {{CollisionRange({{3.0 / 8.0 * M_PI, 5.0 / 8.0 * M_PI}}),
                  CollisionRange({{4.0 / 8.0 * M_PI, 6.0 / 8.0 * M_PI}}),
                  CollisionRange({{1.0 / 8.0 * M_PI, 2.0 / 8.0 * M_PI}})}}),
            PlacementRange({{2.0 * M_PI, 0.0}})));

    // ignore collision within ignore range
    EXPECT_EQ(
        CollisionRange({{5.0 / 4.0 * M_PI, 6.0 / 4.0 * M_PI}}),
        mergeCollisions(CollisionList({{CollisionRange({{M_PI / 2, M_PI}}),
                                        CollisionRange({{5.0 / 4.0 * M_PI,
                                                         6.0 / 4.0 * M_PI}})}}),
                        PlacementRange({{0, M_PI}})));

    // crop collision that ends within ignore range
    EXPECT_EQ(CollisionRange({{1.0 / 2.0 * M_PI, 3.0 / 4.0 * M_PI}}),
              mergeCollisions(
                  CollisionList({{CollisionRange({{1.0 / 2.0 * M_PI, M_PI}})}}),
                  PlacementRange({{3.0 / 4.0 * M_PI, 3.0 / 2.0 * M_PI}})));

    // crop collision that starts within ignore range
    EXPECT_EQ(CollisionRange({{3.0 / 4.0 * M_PI, M_PI}}),
              mergeCollisions(
                  CollisionList({{CollisionRange({{1.0 / 2.0 * M_PI, M_PI}})}}),
                  PlacementRange({{1.0 / 4.0 * M_PI, 3.0 / 4.0 * M_PI}})));
}

TEST(RotationRange, rotatingFixedCollisions) {
    // returns collisions

    auto collisions = rotatingFixedCollisions(
        CollisionRect{CollisionPoint{-1, 0}, CollisionPoint{0, 1}},
        CollisionRect{CollisionPoint{1.4142, -10}, CollisionPoint{10, 10}});

    EXPECT_EQ(1, collisions.size());
    EXPECT_EQ(135, round(deg(collisions.front()[0])));
    EXPECT_EQ(135, round(deg(collisions.front()[1])));
}

TEST(RotationRange, cornerBoxCollisions) {
    {
        // returns intersections in sorted order as angles 0..2PI
        CollisionList list;
        cornerBoxCollisions(
            std::back_inserter(list), CollisionPoint{1, 1},
            CollisionCorners{{CollisionPoint{0, 0},   CollisionPoint{0, 10},
                              CollisionPoint{10, 10}, CollisionPoint{10, 0}}});
        EXPECT_EQ(1, list.size());
        EXPECT_EQ((CollisionRange{{M_PI / 4.0, M_PI * 7.0 / 4.0}}), list[0]);
    }

    {
        // handles no intersections
        CollisionList list;
        cornerBoxCollisions(
            std::back_inserter(list), CollisionPoint{200, 200},
            CollisionCorners{{CollisionPoint{1, 1},   CollisionPoint{1, 10},
                              CollisionPoint{10, 10}, CollisionPoint{10, 1}}});
        EXPECT_EQ(0, list.size());
    }
}

TEST(RotationRange, circleEdgeCollisions) {
    {
        // handles two intersection points
        CollisionAngles list;
        circleEdgeCollisions(std::back_inserter(list), CollisionPoint{0, 1}, 1,
                             CollisionPoint{-10, 0}, CollisionPoint{10, 0});
        std::sort(list.begin(), list.end());
        EXPECT_EQ(2, list.size());
        EXPECT_EQ(static_cast<float>(M_PI / 2), list[0]);
        EXPECT_EQ(static_cast<float>(M_PI * 3.0 / 2.0), list[1]);
    }

    {
        // handles one intersection point
        CollisionAngles list;
        circleEdgeCollisions(std::back_inserter(list), CollisionPoint{0, 1}, 1,
                             CollisionPoint{0, 0}, CollisionPoint{10, 0});
        EXPECT_EQ(1, list.size());
        EXPECT_EQ(static_cast<float>(M_PI / 2), list[0]);
    }

    {
        // only returns intersections within the line segment
        CollisionAngles list;
        circleEdgeCollisions(std::back_inserter(list), CollisionPoint{0, 1}, 1,
                             CollisionPoint{3, 1}, CollisionPoint{30, 1});
        EXPECT_EQ(0, list.size());
    }

    {
        // doesnt count tangetial intersections as collisions
        CollisionAngles list;
        circleEdgeCollisions(std::back_inserter(list), CollisionPoint{0, 1}, 1,
                             CollisionPoint{-10, 1}, CollisionPoint{10, 1});
        EXPECT_EQ(0, list.size());
    }
}

TEST(RotationRange, rotatingRotatingCollisions) {
    {
        // basically works
        CollisionList c = rotatingRotatingCollisions(
            CollisionRect{{-1, 0}, {1, 0}}, CollisionRect{{-1, 0}, {1, 0}},
            CollisionAnchor{1, 1});

        EXPECT_EQ(2, c.size());
        EXPECT_EQ(135, round(deg(c[0][0])));
        EXPECT_EQ(135, round(deg(c[0][1])));
        EXPECT_EQ(315, round(deg(c[1][0])));
        EXPECT_EQ(315, round(deg(c[1][1])));
    }

    {
        // checks if the two boxes are close enough to collide at that angle
        CollisionList c = rotatingRotatingCollisions(
            CollisionRect{{-1, 0}, {1, 0}}, CollisionRect{{-1, 0}, {1, 0}},
            CollisionAnchor{2, 2});

        EXPECT_EQ(0, c.size());
    }
}
