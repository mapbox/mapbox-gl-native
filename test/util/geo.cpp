#include "../fixtures/util.hpp"

#include <mbgl/util/geo.hpp>
#include <mbgl/map/tile_id.hpp>

using namespace mbgl;

TEST(Geo, LatLngFromTileID) {
    for (int i = 0; i < 20; i++) {
        const LatLng ll{ TileID(i, 0, 0, 0) };
        ASSERT_DOUBLE_EQ(-180, ll.longitude);
        ASSERT_DOUBLE_EQ(85.051128779806604, ll.latitude);
    }

    {
        const LatLng ll{ TileID(0, 1, 0, 0) };
        ASSERT_DOUBLE_EQ(180, ll.longitude);
        ASSERT_DOUBLE_EQ(85.051128779806604, ll.latitude);
    }

    {
        const LatLng ll{ TileID(0, -1, 0, 0) };
        ASSERT_DOUBLE_EQ(-540, ll.longitude);
        ASSERT_DOUBLE_EQ(85.051128779806604, ll.latitude);
    }
}


TEST(Geo, LatLngBoundsFromTileID) {
    {
        const LatLngBounds bounds{ TileID(0, 0, 0, 0) };
        ASSERT_DOUBLE_EQ(-180, bounds.sw.longitude);
        ASSERT_DOUBLE_EQ(-85.051128779806604, bounds.sw.latitude);
        ASSERT_DOUBLE_EQ(180, bounds.ne.longitude);
        ASSERT_DOUBLE_EQ(85.051128779806604, bounds.ne.latitude);
    }

    {
        const LatLngBounds bounds{ TileID(1, 0, 1, 0) };
        ASSERT_DOUBLE_EQ(-180, bounds.sw.longitude);
        ASSERT_DOUBLE_EQ(-85.051128779806604, bounds.sw.latitude);
        ASSERT_DOUBLE_EQ(0, bounds.ne.longitude);
        ASSERT_DOUBLE_EQ(0, bounds.ne.latitude);
    }

    {
        const LatLngBounds bounds{ TileID(1, 1, 1, 0) };
        ASSERT_DOUBLE_EQ(0, bounds.sw.longitude);
        ASSERT_DOUBLE_EQ(-85.051128779806604, bounds.sw.latitude);
        ASSERT_DOUBLE_EQ(180, bounds.ne.longitude);
        ASSERT_DOUBLE_EQ(0, bounds.ne.latitude);
    }

    {
        const LatLngBounds bounds{ TileID(1, 0, 0, 0) };
        ASSERT_DOUBLE_EQ(-180, bounds.sw.longitude);
        ASSERT_DOUBLE_EQ(0, bounds.sw.latitude);
        ASSERT_DOUBLE_EQ(0, bounds.ne.longitude);
        ASSERT_DOUBLE_EQ(85.051128779806604, bounds.ne.latitude);
    }
}
