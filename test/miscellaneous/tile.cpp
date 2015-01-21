#include <iostream>
#include "../util.hpp"

#include <mbgl/map/tile.hpp>

using namespace mbgl;


TEST(Variant, isChild) {
    ASSERT_TRUE(Tile::ID(1, 0, 0).isChildOf(Tile::ID(0, 0, 0)));
    ASSERT_TRUE(Tile::ID(1, 1, 0).isChildOf(Tile::ID(0, 0, 0)));
    ASSERT_TRUE(Tile::ID(1, 2, 0).isChildOf(Tile::ID(0, 1, 0)));
    ASSERT_TRUE(Tile::ID(1, 3, 0).isChildOf(Tile::ID(0, 1, 0)));
    ASSERT_TRUE(Tile::ID(1, 4, 0).isChildOf(Tile::ID(0, 2, 0)));
    ASSERT_TRUE(Tile::ID(1, 5, 0).isChildOf(Tile::ID(0, 2, 0)));
    ASSERT_TRUE(Tile::ID(2, 0, 0).isChildOf(Tile::ID(0, 0, 0)));

    ASSERT_TRUE(Tile::ID(2, 8, 0).isChildOf(Tile::ID(0, 2, 0)));
    ASSERT_TRUE(Tile::ID(2, 9, 0).isChildOf(Tile::ID(0, 2, 0)));
    ASSERT_TRUE(Tile::ID(2, 10, 0).isChildOf(Tile::ID(0, 2, 0)));
    ASSERT_TRUE(Tile::ID(2, 11, 0).isChildOf(Tile::ID(0, 2, 0)));
    ASSERT_TRUE(Tile::ID(2, 12, 0).isChildOf(Tile::ID(0, 3, 0)));
    ASSERT_TRUE(Tile::ID(2, 13, 0).isChildOf(Tile::ID(0, 3, 0)));

    ASSERT_TRUE(Tile::ID(1, -1, 0).isChildOf(Tile::ID(0, -1, 0)));
    ASSERT_TRUE(Tile::ID(1, -2, 0).isChildOf(Tile::ID(0, -1, 0)));
    ASSERT_TRUE(Tile::ID(1, -3, 0).isChildOf(Tile::ID(0, -2, 0)));
    ASSERT_TRUE(Tile::ID(1, -4, 0).isChildOf(Tile::ID(0, -2, 0)));
    ASSERT_TRUE(Tile::ID(2, -1, 0).isChildOf(Tile::ID(0, -1, 0)));
    ASSERT_TRUE(Tile::ID(2, -2, 0).isChildOf(Tile::ID(0, -1, 0)));
    ASSERT_TRUE(Tile::ID(2, -3, 0).isChildOf(Tile::ID(0, -1, 0)));
    ASSERT_TRUE(Tile::ID(2, -4, 0).isChildOf(Tile::ID(0, -1, 0)));
    ASSERT_TRUE(Tile::ID(2, -5, 0).isChildOf(Tile::ID(0, -2, 0)));
    ASSERT_TRUE(Tile::ID(2, -6, 0).isChildOf(Tile::ID(0, -2, 0)));
    ASSERT_TRUE(Tile::ID(2, -7, 0).isChildOf(Tile::ID(0, -2, 0)));
    ASSERT_TRUE(Tile::ID(2, -8, 0).isChildOf(Tile::ID(0, -2, 0)));

    ASSERT_FALSE(Tile::ID(4, -16, 0).isChildOf(Tile::ID(0, -2, 0)));
    ASSERT_TRUE(Tile::ID(4, -17, 0).isChildOf(Tile::ID(0, -2, 0)));

    ASSERT_TRUE(Tile::ID(2, -1, 0).isChildOf(Tile::ID(1, -1, 0)));
    ASSERT_TRUE(Tile::ID(2, -2, 0).isChildOf(Tile::ID(1, -1, 0)));
    ASSERT_TRUE(Tile::ID(2, -3, 0).isChildOf(Tile::ID(1, -2, 0)));
    ASSERT_TRUE(Tile::ID(2, -4, 0).isChildOf(Tile::ID(1, -2, 0)));
    ASSERT_TRUE(Tile::ID(3, -1, 0).isChildOf(Tile::ID(1, -1, 0)));
    ASSERT_TRUE(Tile::ID(3, -2, 0).isChildOf(Tile::ID(1, -1, 0)));
    ASSERT_TRUE(Tile::ID(3, -3, 0).isChildOf(Tile::ID(1, -1, 0)));
    ASSERT_TRUE(Tile::ID(3, -4, 0).isChildOf(Tile::ID(1, -1, 0)));
    ASSERT_TRUE(Tile::ID(3, -5, 0).isChildOf(Tile::ID(1, -2, 0)));
}
