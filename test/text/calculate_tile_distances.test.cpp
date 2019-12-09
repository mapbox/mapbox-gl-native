#include <mbgl/test/util.hpp>

#include <mbgl/geometry/anchor.hpp>
#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

using namespace mbgl;

TEST(calculateTileDistances, Point) {
    const GeometryCoordinates line = {Point<int16_t>{1, 1}};
    const auto distances = SymbolLayout::calculateTileDistances(line, Anchor(1.0f, 1.0f, .0f, 0u));
    EXPECT_EQ(distances.size(), 1u);
    EXPECT_EQ(distances[0], .0f);
}

TEST(calculateTileDistances, Line) {
    const GeometryCoordinates line = {
        Point<int16_t>{1, 1}, Point<int16_t>{1, 2}, Point<int16_t>{1, 3}, Point<int16_t>{1, 4}};
    const auto distances = SymbolLayout::calculateTileDistances(line, Anchor(1.0f, 3.0f, .0f, 2u));
    EXPECT_EQ(distances, std::vector<float>({2.0f, 1.0f, 0.0f, 1.0f}));
}

TEST(calculateTileDistances, EmptySegment) {
    const GeometryCoordinates line = {};
    const auto distances = SymbolLayout::calculateTileDistances(line, Anchor(1.0f, 1.0f, .0f));
    EXPECT_EQ(distances.size(), 0u);
}
