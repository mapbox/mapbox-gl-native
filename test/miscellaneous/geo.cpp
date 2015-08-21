#include "../fixtures/util.hpp"

#include <mbgl/util/geo.hpp>
#include <mbgl/map/tile_id.hpp>

using namespace mbgl;

TEST(Geo, LatLngFromTileID) {
    for (int i = 0; i < 20; i++) {
        const LatLng ll{ TileID(i, 0, 0, 0) };
        ASSERT_DOUBLE_EQ(-180, ll.latitude);
        ASSERT_DOUBLE_EQ(85.051128779806604, ll.longitude);
    }

    {
        const LatLng ll{ TileID(0, 1, 0, 0) };
        ASSERT_DOUBLE_EQ(180, ll.latitude);
        ASSERT_DOUBLE_EQ(85.051128779806604, ll.longitude);
    }

    {
        const LatLng ll{ TileID(0, -1, 0, 0) };
        ASSERT_DOUBLE_EQ(-540, ll.latitude);
        ASSERT_DOUBLE_EQ(85.051128779806604, ll.longitude);
    }
}


TEST(Geo, LatLngBoundsFromTileID) {
    {
        const LatLngBounds bounds{ TileID(0, 0, 0, 0) };
        ASSERT_DOUBLE_EQ(-180, bounds.sw.latitude);
        ASSERT_DOUBLE_EQ(-85.051128779806604, bounds.sw.longitude);
        ASSERT_DOUBLE_EQ(180, bounds.ne.latitude);
        ASSERT_DOUBLE_EQ(85.051128779806604, bounds.ne.longitude);
    }

    {
        const LatLngBounds bounds{ TileID(1, 0, 1, 0) };
        ASSERT_DOUBLE_EQ(-180, bounds.sw.latitude);
        ASSERT_DOUBLE_EQ(-85.051128779806604, bounds.sw.longitude);
        ASSERT_DOUBLE_EQ(0, bounds.ne.latitude);
        ASSERT_DOUBLE_EQ(0, bounds.ne.longitude);
    }

    {
        const LatLngBounds bounds{ TileID(1, 1, 1, 0) };
        ASSERT_DOUBLE_EQ(0, bounds.sw.latitude);
        ASSERT_DOUBLE_EQ(-85.051128779806604, bounds.sw.longitude);
        ASSERT_DOUBLE_EQ(180, bounds.ne.latitude);
        ASSERT_DOUBLE_EQ(0, bounds.ne.longitude);
    }

    {
        const LatLngBounds bounds{ TileID(1, 0, 0, 0) };
        ASSERT_DOUBLE_EQ(-180, bounds.sw.latitude);
        ASSERT_DOUBLE_EQ(0, bounds.sw.longitude);
        ASSERT_DOUBLE_EQ(0, bounds.ne.latitude);
        ASSERT_DOUBLE_EQ(85.051128779806604, bounds.ne.longitude);
    }
}
