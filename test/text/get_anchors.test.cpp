#include <mbgl/test/util.hpp>

#include <mbgl/text/get_anchors.hpp>

using namespace mbgl;

namespace mbgl {
constexpr inline bool operator==(const Anchor& rhs, const Anchor& lhs) {
    return rhs.point == lhs.point && rhs.angle == lhs.angle && rhs.segment == lhs.segment;
}
} // namespace mbgl

namespace {
const auto makeLine = [](std::size_t shift = 0u) {
    GeometryCoordinates line;
    for (std::size_t i = shift; i < 10u + shift; ++i) {
        line.push_back({1, static_cast<int16_t>(i)});
    }
    return line;
};

const GeometryCoordinates continuedLine = makeLine();
const GeometryCoordinates nonContinuedLine = makeLine(1u);

const float smallSpacing = 2.0f;
const float bigSpacing = 3.0f;
const float textLeft = -1.0f;
const float textRight = 1.0f;
const float iconLeft = -0.5f;
const float iconRight = 0.5f;
const float glyphSize = 0.1f;
const float boxScale = 1.0f;
const float overscaling = 1.0f;
} // namespace

TEST(getAnchors, NonContinuedLineShortLabels) {
    const Anchors anchors = getAnchors(
        nonContinuedLine, bigSpacing, M_PI, textLeft, textRight, iconLeft, iconRight, glyphSize, boxScale, overscaling);
    const Anchors expected_anchors = {Anchor(1.0f, 2.0f, 1.570796371f, 1u),
                                      Anchor(1.0f, 5.0f, 1.570796371f, 4u),
                                      Anchor(1.0f, 8.0f, 1.570796371f, 7u)};

    EXPECT_EQ(anchors, expected_anchors);
}

TEST(getAnchors, NonContinuedLineLongLabels) {
    const Anchors anchors = getAnchors(nonContinuedLine,
                                       smallSpacing,
                                       M_PI,
                                       textLeft,
                                       textRight,
                                       iconLeft,
                                       iconRight,
                                       glyphSize,
                                       boxScale,
                                       overscaling);
    const Anchors expected_anchors = {Anchor(1.0f, 2.0f, 1.570796371f, 1u),
                                      Anchor(1.0f, 5.0f, 1.570796371f, 3u),
                                      Anchor(1.0f, 7.0f, 1.570796371f, 6u)};

    EXPECT_EQ(anchors, expected_anchors);
}

TEST(getAnchors, ContinuedLineShortLabels) {
    const Anchors anchors = getAnchors(
        continuedLine, bigSpacing, M_PI, textLeft, textRight, iconLeft, iconRight, glyphSize, boxScale, overscaling);
    const Anchors expected_anchors = {Anchor(1.0f, 2.0f, 1.570796371f, 1u),
                                      Anchor(1.0f, 5.0f, 1.570796371f, 4u),
                                      Anchor(1.0f, 8.0f, 1.570796371f, 7u)};

    EXPECT_EQ(anchors, expected_anchors);
}

TEST(getAnchors, ContinuedLineLongLabels) {
    const Anchors anchors = getAnchors(
        continuedLine, smallSpacing, M_PI, textLeft, textRight, iconLeft, iconRight, glyphSize, boxScale, overscaling);
    const Anchors expected_anchors = {Anchor(1.0f, 1.0f, 1.570796371f, 1u),
                                      Anchor(1.0f, 4.0f, 1.570796371f, 3u),
                                      Anchor(1.0f, 6.0f, 1.570796371f, 6u)};

    EXPECT_EQ(anchors, expected_anchors);
}

TEST(getAnchors, OverscaledAnchorsInParent) {
    const Anchors anchors = getAnchors(
        nonContinuedLine, bigSpacing, M_PI, textLeft, textRight, iconLeft, iconRight, glyphSize, boxScale, overscaling);
    const Anchors childAnchors = getAnchors(nonContinuedLine,
                                            bigSpacing / 2,
                                            M_PI,
                                            textLeft,
                                            textRight,
                                            iconLeft,
                                            iconRight,
                                            glyphSize,
                                            0.5f /*boxScale*/,
                                            2.0f /*overscaling*/);
    for (const auto& anchor : anchors) {
        EXPECT_TRUE(std::find(childAnchors.begin(), childAnchors.end(), anchor) != childAnchors.end());
    }
}

TEST(getAnchors, UseMidpointForShortLine) {
    const GeometryCoordinates shortLine = {Point<int16_t>{1, 1}, Point<int16_t>{1, 3}};
    const Anchors anchors = getAnchors(
        shortLine, smallSpacing, M_PI, textLeft, textRight, iconLeft, iconRight, glyphSize, boxScale, overscaling);
    const Anchors expected_anchors = {Anchor(1.0f, 2.0f, 1.570796371f, 0u)};

    EXPECT_EQ(anchors, expected_anchors);
}

TEST(getAnchors, GetCenterAnchor) {
    const GeometryCoordinates line = {
        Point<int16_t>{1, 1}, Point<int16_t>{1, 3}, Point<int16_t>{3, 6}, Point<int16_t>{4, 7}};
    const auto anchor = getCenterAnchor(line, M_PI, textLeft, textRight, iconLeft, iconRight, glyphSize, boxScale);
    EXPECT_TRUE(anchor);
    EXPECT_EQ(*anchor, Anchor(2.0f, 4.0f, .982793748f, 1u));
}

TEST(getAnchors, GetCenterAnchorOutsideTileBounds) {
    const GeometryCoordinates line = {Point<int16_t>{-10, -10}, Point<int16_t>{5, 5}};
    const auto anchor = getCenterAnchor(line, M_PI, textLeft, textRight, iconLeft, iconRight, glyphSize, boxScale);
    EXPECT_TRUE(anchor);
    EXPECT_EQ(*anchor, Anchor(-3.0f, -3.0f, .785398185f, 0u));
}

TEST(getAnchors, GetCenterAnchorFailMaxAngle) {
    const GeometryCoordinates line = {Point<int16_t>{1, 1}, Point<int16_t>{1, 3}, Point<int16_t>{3, 3}};
    const auto anchor = getCenterAnchor(line, M_PI / 4, textLeft, textRight, iconLeft, iconRight, glyphSize, boxScale);
    EXPECT_FALSE(anchor);
}
