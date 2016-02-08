#include "../fixtures/util.hpp"

#include <mbgl/util/geo.hpp>
#include <mbgl/map/tile_id.hpp>

using namespace mbgl;

TEST(LatLngBounds, World) {
    auto result = LatLngBounds::world();
    ASSERT_DOUBLE_EQ(-90,  result.south());
    ASSERT_DOUBLE_EQ( 90,  result.north());
    ASSERT_DOUBLE_EQ(-180, result.west());
    ASSERT_DOUBLE_EQ( 180, result.east());
}

TEST(LatLngBounds, Singleton) {
    auto result = LatLngBounds::singleton({1, 2});
    ASSERT_DOUBLE_EQ(1, result.south());
    ASSERT_DOUBLE_EQ(1, result.north());
    ASSERT_DOUBLE_EQ(2, result.west());
    ASSERT_DOUBLE_EQ(2, result.east());
}

TEST(LatLngBounds, Hull) {
    double s = 1, w = 2, n = 3, e = 4;

    auto swne = LatLngBounds::hull({s, w}, {n, e});
    ASSERT_DOUBLE_EQ(s, swne.south());
    ASSERT_DOUBLE_EQ(n, swne.north());
    ASSERT_DOUBLE_EQ(w, swne.west());
    ASSERT_DOUBLE_EQ(e, swne.east());

    auto nesw = LatLngBounds::hull({n, e}, {s, w});
    ASSERT_DOUBLE_EQ(s, nesw.south());
    ASSERT_DOUBLE_EQ(n, nesw.north());
    ASSERT_DOUBLE_EQ(w, nesw.west());
    ASSERT_DOUBLE_EQ(e, nesw.east());

    auto senw = LatLngBounds::hull({s, e}, {n, w});
    ASSERT_DOUBLE_EQ(s, senw.south());
    ASSERT_DOUBLE_EQ(n, senw.north());
    ASSERT_DOUBLE_EQ(w, senw.west());
    ASSERT_DOUBLE_EQ(e, senw.east());

    auto nwse = LatLngBounds::hull({n, w}, {s, e});
    ASSERT_DOUBLE_EQ(s, nwse.south());
    ASSERT_DOUBLE_EQ(n, nwse.north());
    ASSERT_DOUBLE_EQ(w, nwse.west());
    ASSERT_DOUBLE_EQ(e, nwse.east());
}

TEST(LatLngBounds, Empty) {
    ASSERT_TRUE(LatLngBounds::empty().isEmpty());
    ASSERT_FALSE(LatLngBounds::world().isEmpty());
}

TEST(LatLngBounds, Center) {
    auto result = LatLngBounds::hull({1, 2}, {3, 4}).center();
    ASSERT_DOUBLE_EQ(2, result.latitude);
    ASSERT_DOUBLE_EQ(3, result.longitude);
}

TEST(LatLngBounds, Southwest) {
    auto result = LatLngBounds::hull({1, 2}, {3, 4}).southwest();
    ASSERT_DOUBLE_EQ(1, result.latitude);
    ASSERT_DOUBLE_EQ(2, result.longitude);
}

TEST(LatLngBounds, Northeast) {
    auto result = LatLngBounds::hull({1, 2}, {3, 4}).northeast();
    ASSERT_DOUBLE_EQ(3, result.latitude);
    ASSERT_DOUBLE_EQ(4, result.longitude);
}

TEST(LatLngBounds, Southeast) {
    auto result = LatLngBounds::hull({1, 2}, {3, 4}).southeast();
    ASSERT_DOUBLE_EQ(1, result.latitude);
    ASSERT_DOUBLE_EQ(4, result.longitude);
}

TEST(LatLngBounds, Northwest) {
    auto result = LatLngBounds::hull({1, 2}, {3, 4}).northwest();
    ASSERT_DOUBLE_EQ(3, result.latitude);
    ASSERT_DOUBLE_EQ(2, result.longitude);
}

TEST(LatLng, FromTileID) {
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

TEST(LatLngBounds, FromTileID) {
    {
        const LatLngBounds bounds{ TileID(0, 0, 0, 0) };
        ASSERT_DOUBLE_EQ(-180, bounds.west());
        ASSERT_DOUBLE_EQ(-85.051128779806604, bounds.south());
        ASSERT_DOUBLE_EQ(180, bounds.east());
        ASSERT_DOUBLE_EQ(85.051128779806604, bounds.north());
    }

    {
        const LatLngBounds bounds{ TileID(1, 0, 1, 0) };
        ASSERT_DOUBLE_EQ(-180, bounds.west());
        ASSERT_DOUBLE_EQ(-85.051128779806604, bounds.south());
        ASSERT_DOUBLE_EQ(0, bounds.east());
        ASSERT_DOUBLE_EQ(0, bounds.north());
    }

    {
        const LatLngBounds bounds{ TileID(1, 1, 1, 0) };
        ASSERT_DOUBLE_EQ(0, bounds.west());
        ASSERT_DOUBLE_EQ(-85.051128779806604, bounds.south());
        ASSERT_DOUBLE_EQ(180, bounds.east());
        ASSERT_DOUBLE_EQ(0, bounds.north());
    }

    {
        const LatLngBounds bounds{ TileID(1, 0, 0, 0) };
        ASSERT_DOUBLE_EQ(-180, bounds.west());
        ASSERT_DOUBLE_EQ(0, bounds.south());
        ASSERT_DOUBLE_EQ(0, bounds.east());
        ASSERT_DOUBLE_EQ(85.051128779806604, bounds.north());
    }
}
