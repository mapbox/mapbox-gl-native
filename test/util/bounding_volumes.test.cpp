#include <mbgl/util/bounding_volumes.hpp>

#include <gtest/gtest.h>
#include <math.h>

using namespace mbgl;

TEST(BoundingVolumes, CreateAabb) {
    util::AABB aabb({0, 0, 0}, {2, 4, 6});

    EXPECT_EQ(aabb.min, (vec3{0, 0, 0}));
    EXPECT_EQ(aabb.max, (vec3{2, 4, 6}));
}

TEST(BoundingVolumes, AabbEquality) {
    util::AABB aabb0({-4.23, 1.01, 2.0}, {-1.0, 1.02, 100.0});
    util::AABB aabb1({-4.23, 1.01, 2.0}, {-1.0, 1.02, 100.0});
    util::AABB aabb2({-4.23, 1.01, 2.0}, {-1.0, 1.03, 55.0});

    EXPECT_EQ(aabb0, aabb1);
    EXPECT_NE(aabb0, aabb2);
}

TEST(BoundingVolumes, CreateAabbQuadrants) {
    util::AABB aabb({0, 0, 0}, {2, 4, 16});

    util::AABB quadrant0({0, 0, 0}, {1, 2, 16});
    util::AABB quadrant1({1, 0, 0}, {2, 2, 16});
    util::AABB quadrant2({0, 2, 0}, {1, 4, 16});
    util::AABB quadrant3({1, 2, 0}, {2, 4, 16});

    EXPECT_EQ(aabb.quadrant(0), quadrant0);
    EXPECT_EQ(aabb.quadrant(1), quadrant1);
    EXPECT_EQ(aabb.quadrant(2), quadrant2);
    EXPECT_EQ(aabb.quadrant(3), quadrant3);
}

TEST(BoundingVolumes, AabbDistanceToPoint) {
    util::AABB aabb({-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0});

    EXPECT_EQ(aabb.distanceXYZ({0.5, -0.5, 0.5}), (vec3{0, 0, 0}));
    EXPECT_EQ(aabb.distanceXYZ({1, 1, 1}), (vec3{0, 0, 0}));
    EXPECT_EQ(aabb.distanceXYZ({0, 10, 0}), (vec3{0, 9, 0}));
    EXPECT_EQ(aabb.distanceXYZ({-2, -2, -4}), (vec3{1, 1, 3}));
}

TEST(BoundingVolumes, AabbAabbIntersection) {
    util::AABB aabb0({1, 0, 0}, {3, 2, 1});
    util::AABB aabb1({2.5, 1.5, -1}, {6.0, 2.5, 1});
    util::AABB aabb2({3.5, -2, -2}, {5.0, 1, -1.5});
    util::AABB aabb3({0.5, -1, -1}, {6.5, 3, 4});

    EXPECT_EQ(aabb0.intersects(aabb1), true);
    EXPECT_EQ(aabb0.intersects(aabb2), false);
    EXPECT_EQ(aabb1.intersects(aabb2), false);
    EXPECT_EQ(aabb3.intersects(aabb0), true);
    EXPECT_EQ(aabb3.intersects(aabb1), true);
    EXPECT_EQ(aabb3.intersects(aabb2), false);
}

TEST(BoundingVolumes, CreateFrustumFromProjectionMatrix) {
    mat4 projMatrix;
    mat4 invProjMatrix;
    matrix::perspective(projMatrix, M_PI_2, 1.0, 0.1, 100.0);
    matrix::invert(invProjMatrix, projMatrix);

    const util::Frustum frustum = util::Frustum::fromInvProjMatrix(invProjMatrix, 1.0, 0.0, false);

    const std::array<vec3, 8> expectedPoints = {vec3{-0.1, 0.1, -0.1},
                                                vec3{0.1, 0.1, -0.1},
                                                vec3{0.1, -0.1, -0.1},
                                                vec3{-0.1, -0.1, -0.1},
                                                vec3{-100.0, 100.0, -100.0},
                                                vec3{100.0, 100.0, -100.0},
                                                vec3{100.0, -100.0, -100.0},
                                                vec3{-100.0, -100.0, -100.0}};

    const std::array<vec4, 6> expectedPlanes = {

        vec4{0, -0.707, 0.707, 0},
        vec4{-0.707, 0, 0.707, 0},
        vec4{0.707, 0, 0.707, 0},
        vec4{0, 0.707, 0.707, 0},
        vec4{0, 0, 1.0, 0.1},
        vec4{0, 0, -1.0, -100.0}};

    const auto roundPoint = [](vec3& v) {
        for (auto& p : v) {
            p = round(p * 10.0) / 10.0;
        }
    };

    const auto roundPlane = [](vec4& v) {
        for (auto& p : v) {
            p = round(p * 1000.0) / 1000.0;
        }
    };

    std::array<vec3, 8> actualPoints = frustum.getPoints();
    std::array<vec4, 6> actualPlanes = frustum.getPlanes();

    std::for_each(actualPoints.begin(), actualPoints.end(), roundPoint);
    std::for_each(actualPlanes.begin(), actualPlanes.end(), roundPlane);

    EXPECT_EQ(actualPoints, expectedPoints);
    EXPECT_EQ(actualPlanes, expectedPlanes);
}

static util::Frustum createTestFrustum(
    double fovy, double aspectRatio, double zNear, double zFar, double elevation, double bearing) {
    mat4 proj;
    mat4 invProj;

    // Create a complete projection matrix for transforming from world space to clip space
    matrix::perspective(proj, fovy, aspectRatio, zNear, zFar);
    matrix::scale(proj, proj, 1, -1, 1);
    matrix::translate(proj, proj, 0, 0, elevation);
    matrix::rotate_z(proj, proj, bearing);
    matrix::invert(invProj, proj);

    return util::Frustum::fromInvProjMatrix(invProj, 1.0, 0.0);
}

TEST(BoundingVolumes, AabbFullyInsideFrustum) {
    const util::Frustum frustum = createTestFrustum(M_PI_2, 1.0, 0.1, 100.0, -5.0, 0.0);

    const std::array<util::AABB, 3> aabbArray = {
        util::AABB({-1, -1, 0}, {1, 1, 0}), util::AABB({-5, -5, 0}, {5, 5, 0}), util::AABB({-5, -5, 0}, {-4, -2, 0})};

    EXPECT_EQ(frustum.intersects(aabbArray[0]), util::IntersectionResult::Contains);
    EXPECT_EQ(frustum.intersects(aabbArray[1]), util::IntersectionResult::Contains);
    EXPECT_EQ(frustum.intersects(aabbArray[2]), util::IntersectionResult::Contains);
}

TEST(BoundingVolumes, AabbIntersectsFrustum) {
    const util::Frustum frustum = createTestFrustum(M_PI_2, 1.0, 0.1, 100.0, -5.0, 0.0);

    const std::array<util::AABB, 2> aabbArray = {util::AABB({-6, -6, 0}, {6, 6, 0}),
                                                 util::AABB({-6, -6, 0}, {-5, -5, 0})};

    EXPECT_EQ(frustum.intersects(aabbArray[0]), util::IntersectionResult::Intersects);
    EXPECT_EQ(frustum.intersects(aabbArray[1]), util::IntersectionResult::Intersects);
}

TEST(BoundingVolumes, AabbIntersectsFrustumEdgeCase) {
    const util::Frustum frustum = createTestFrustum(M_PI_2, 1.0, 0.1, 100.0, -5.0, M_PI_4);
    const util::AABB aabb({-10, 10, 0}, {10, 12, 0});

    // Intersection test should report intersection even though shapes are separate
    EXPECT_EQ(frustum.intersects(aabb), util::IntersectionResult::Intersects);
    EXPECT_EQ(frustum.intersectsPrecise(aabb), util::IntersectionResult::Separate);
}