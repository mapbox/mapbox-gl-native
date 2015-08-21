#include <iostream>
#include "../fixtures/util.hpp"

#include <mbgl/map/tile_id.hpp>

using namespace mbgl;


TEST(Variant, isChild) {
    ASSERT_TRUE(TileID(1, 0, 0, 1).isChildOf(TileID(0, 0, 0, 0)));
    ASSERT_TRUE(TileID(1, 1, 0, 1).isChildOf(TileID(0, 0, 0, 0)));
    ASSERT_TRUE(TileID(1, 2, 0, 1).isChildOf(TileID(0, 1, 0, 0)));
    ASSERT_TRUE(TileID(1, 3, 0, 1).isChildOf(TileID(0, 1, 0, 0)));
    ASSERT_TRUE(TileID(1, 4, 0, 1).isChildOf(TileID(0, 2, 0, 0)));
    ASSERT_TRUE(TileID(1, 5, 0, 1).isChildOf(TileID(0, 2, 0, 0)));
    ASSERT_TRUE(TileID(2, 0, 0, 2).isChildOf(TileID(0, 0, 0, 0)));

    ASSERT_TRUE(TileID(2, 8, 0, 2).isChildOf(TileID(0, 2, 0, 0)));
    ASSERT_TRUE(TileID(2, 9, 0, 2).isChildOf(TileID(0, 2, 0, 0)));
    ASSERT_TRUE(TileID(2, 10, 0, 2).isChildOf(TileID(0, 2, 0, 0)));
    ASSERT_TRUE(TileID(2, 11, 0, 2).isChildOf(TileID(0, 2, 0, 0)));
    ASSERT_TRUE(TileID(2, 12, 0, 2).isChildOf(TileID(0, 3, 0, 0)));
    ASSERT_TRUE(TileID(2, 13, 0, 2).isChildOf(TileID(0, 3, 0, 0)));

    ASSERT_TRUE(TileID(1, -1, 0, 1).isChildOf(TileID(0, -1, 0, 0)));
    ASSERT_TRUE(TileID(1, -2, 0, 1).isChildOf(TileID(0, -1, 0, 0)));
    ASSERT_TRUE(TileID(1, -3, 0, 1).isChildOf(TileID(0, -2, 0, 0)));
    ASSERT_TRUE(TileID(1, -4, 0, 1).isChildOf(TileID(0, -2, 0, 0)));
    ASSERT_TRUE(TileID(2, -1, 0, 2).isChildOf(TileID(0, -1, 0, 0)));
    ASSERT_TRUE(TileID(2, -2, 0, 2).isChildOf(TileID(0, -1, 0, 0)));
    ASSERT_TRUE(TileID(2, -3, 0, 2).isChildOf(TileID(0, -1, 0, 0)));
    ASSERT_TRUE(TileID(2, -4, 0, 2).isChildOf(TileID(0, -1, 0, 0)));
    ASSERT_TRUE(TileID(2, -5, 0, 2).isChildOf(TileID(0, -2, 0, 0)));
    ASSERT_TRUE(TileID(2, -6, 0, 2).isChildOf(TileID(0, -2, 0, 0)));
    ASSERT_TRUE(TileID(2, -7, 0, 2).isChildOf(TileID(0, -2, 0, 0)));
    ASSERT_TRUE(TileID(2, -8, 0, 2).isChildOf(TileID(0, -2, 0, 0)));

    ASSERT_FALSE(TileID(4, -16, 0, 4).isChildOf(TileID(0, -2, 0, 0)));
    ASSERT_TRUE(TileID(4, -17, 0, 4).isChildOf(TileID(0, -2, 0, 0)));

    ASSERT_TRUE(TileID(2, -1, 0, 2).isChildOf(TileID(1, -1, 0, 1)));
    ASSERT_TRUE(TileID(2, -2, 0, 2).isChildOf(TileID(1, -1, 0, 1)));
    ASSERT_TRUE(TileID(2, -3, 0, 2).isChildOf(TileID(1, -2, 0, 1)));
    ASSERT_TRUE(TileID(2, -4, 0, 2).isChildOf(TileID(1, -2, 0, 1)));
    ASSERT_TRUE(TileID(3, -1, 0, 3).isChildOf(TileID(1, -1, 0, 1)));
    ASSERT_TRUE(TileID(3, -2, 0, 3).isChildOf(TileID(1, -1, 0, 1)));
    ASSERT_TRUE(TileID(3, -3, 0, 3).isChildOf(TileID(1, -1, 0, 1)));
    ASSERT_TRUE(TileID(3, -4, 0, 3).isChildOf(TileID(1, -1, 0, 1)));
    ASSERT_TRUE(TileID(3, -5, 0, 3).isChildOf(TileID(1, -2, 0, 1)));
}
