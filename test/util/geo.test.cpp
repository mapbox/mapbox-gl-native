#include <mbgl/test/util.hpp>

#include <mbgl/util/constants.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/tile/tile_id.hpp>

using namespace mbgl;

TEST(LatLng, InvalidLatLng) {
    try {
        LatLng { NAN };
        ASSERT_TRUE(false) << "should throw";
    } catch (const std::domain_error& error) {
        ASSERT_EQ(std::string(error.what()), "latitude must not be NaN");
    }
    try {
        LatLng { 0, NAN };
        ASSERT_TRUE(false) << "should throw";
    } catch (const std::domain_error& error) {
        ASSERT_EQ(std::string(error.what()), "longitude must not be NaN");
    }
    try {
        LatLng { 91.0 };
        ASSERT_TRUE(false) << "should throw";
    } catch (const std::domain_error& error) {
        ASSERT_EQ(std::string(error.what()), "latitude must be between -90 and 90");
    }
    try {
        LatLng { 0, std::numeric_limits<double>::infinity() };
        ASSERT_TRUE(false) << "should throw";
    } catch (const std::domain_error& error) {
        ASSERT_EQ(std::string(error.what()), "longitude must not be infinite");
    }
}

TEST(EdgeInsets, InvalidEdgeInsets) {
    try {
        EdgeInsets { NAN };
        ASSERT_TRUE(false) << "should throw";
    } catch (const std::domain_error& error) {
        ASSERT_EQ(std::string(error.what()), "top must not be NaN");
    }
    try {
        EdgeInsets { 0, NAN };
        ASSERT_TRUE(false) << "should throw";
    } catch (const std::domain_error& error) {
        ASSERT_EQ(std::string(error.what()), "left must not be NaN");
    }
    try {
        EdgeInsets { 0, 0, NAN };
        ASSERT_TRUE(false) << "should throw";
    } catch (const std::domain_error& error) {
        ASSERT_EQ(std::string(error.what()), "bottom must not be NaN");
    }
    try {
        EdgeInsets { 0, 0, 0, NAN };
        ASSERT_TRUE(false) << "should throw";
    } catch (const std::domain_error& error) {
        ASSERT_EQ(std::string(error.what()), "right must not be NaN");
    }
}

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
    ASSERT_DOUBLE_EQ(2, result.latitude());
    ASSERT_DOUBLE_EQ(3, result.longitude());
}

TEST(LatLngBounds, Southwest) {
    auto result = LatLngBounds::hull({1, 2}, {3, 4}).southwest();
    ASSERT_DOUBLE_EQ(1, result.latitude());
    ASSERT_DOUBLE_EQ(2, result.longitude());
}

TEST(LatLngBounds, Northeast) {
    auto result = LatLngBounds::hull({1, 2}, {3, 4}).northeast();
    ASSERT_DOUBLE_EQ(3, result.latitude());
    ASSERT_DOUBLE_EQ(4, result.longitude());
}

TEST(LatLngBounds, Southeast) {
    auto result = LatLngBounds::hull({1, 2}, {3, 4}).southeast();
    ASSERT_DOUBLE_EQ(1, result.latitude());
    ASSERT_DOUBLE_EQ(4, result.longitude());
}

TEST(LatLngBounds, Northwest) {
    auto result = LatLngBounds::hull({1, 2}, {3, 4}).northwest();
    ASSERT_DOUBLE_EQ(3, result.latitude());
    ASSERT_DOUBLE_EQ(2, result.longitude());
}

TEST(LatLng, FromTileID) {
    for (int i = 0; i < 20; i++) {
        const LatLng ll{ CanonicalTileID(i, 0, 0) };
        ASSERT_DOUBLE_EQ(-util::LONGITUDE_MAX, ll.longitude());
        ASSERT_DOUBLE_EQ(util::LATITUDE_MAX, ll.latitude());
    }

    {
        const LatLng ll{ UnwrappedTileID(0, 1, 0) };
        ASSERT_DOUBLE_EQ(util::LONGITUDE_MAX, ll.longitude());
        ASSERT_DOUBLE_EQ(util::LATITUDE_MAX, ll.latitude());
    }

    {
        const LatLng ll{ UnwrappedTileID(0, -1, 0) };
        ASSERT_DOUBLE_EQ(-540, ll.longitude());
        ASSERT_DOUBLE_EQ(util::LATITUDE_MAX, ll.latitude());
    }
}

TEST(LatLng, Boundaries) {
    LatLng coordinate;
    ASSERT_DOUBLE_EQ(0, coordinate.latitude());
    ASSERT_DOUBLE_EQ(0, coordinate.longitude());

    coordinate = LatLng(0, -180.1);
    ASSERT_DOUBLE_EQ(-180.1, coordinate.longitude());

    coordinate.wrap();
    ASSERT_DOUBLE_EQ(179.90000000000001, coordinate.longitude()); // 1E-14

    coordinate = LatLng(0, 180.9);
    coordinate.wrap();
    ASSERT_DOUBLE_EQ(-179.09999999999999, coordinate.longitude());

    coordinate = LatLng(0, -360.5);
    coordinate.wrap();
    ASSERT_DOUBLE_EQ(-0.5, coordinate.longitude());

    coordinate = LatLng(0, 360.5);
    coordinate.wrap();
    ASSERT_DOUBLE_EQ(0.5, coordinate.longitude());

    coordinate = LatLng(0, 360000.5);
    coordinate.wrap();
    ASSERT_DOUBLE_EQ(0.5, coordinate.longitude());
}

TEST(LatLngBounds, FromTileID) {
    {
        const LatLngBounds bounds{ CanonicalTileID(0, 0, 0) };
        ASSERT_DOUBLE_EQ(-util::LONGITUDE_MAX, bounds.west());
        ASSERT_DOUBLE_EQ(-util::LATITUDE_MAX, bounds.south());
        ASSERT_DOUBLE_EQ(util::LONGITUDE_MAX, bounds.east());
        ASSERT_DOUBLE_EQ(util::LATITUDE_MAX, bounds.north());
    }

    {
        const LatLngBounds bounds{ CanonicalTileID(1, 0, 1) };
        ASSERT_DOUBLE_EQ(-util::LONGITUDE_MAX, bounds.west());
        ASSERT_DOUBLE_EQ(-util::LATITUDE_MAX, bounds.south());
        ASSERT_DOUBLE_EQ(0, bounds.east());
        ASSERT_DOUBLE_EQ(0, bounds.north());
    }

    {
        const LatLngBounds bounds{ CanonicalTileID(1, 1, 1) };
        ASSERT_DOUBLE_EQ(0, bounds.west());
        ASSERT_DOUBLE_EQ(-util::LATITUDE_MAX, bounds.south());
        ASSERT_DOUBLE_EQ(util::LONGITUDE_MAX, bounds.east());
        ASSERT_DOUBLE_EQ(0, bounds.north());
    }

    {
        const LatLngBounds bounds{ CanonicalTileID(1, 0, 0) };
        ASSERT_DOUBLE_EQ(-util::LONGITUDE_MAX, bounds.west());
        ASSERT_DOUBLE_EQ(0, bounds.south());
        ASSERT_DOUBLE_EQ(0, bounds.east());
        ASSERT_DOUBLE_EQ(util::LATITUDE_MAX, bounds.north());
    }
}

TEST(LatLngBounds, Contains) {
    const LatLngBounds bounds( CanonicalTileID(4,2,6));
    const LatLngBounds innerBounds( CanonicalTileID(9,82,197));
    EXPECT_TRUE(bounds.contains(innerBounds));
}
