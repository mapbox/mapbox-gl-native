#include <mbgl/test/util.hpp>

#include <mbgl/util/constants.hpp>
#include <mbgl/util/projection.hpp>

#include <limits>

using namespace mbgl;

TEST(Projection, MetersPerPixelAtLatitude) {
    double zoom = 0;
    EXPECT_DOUBLE_EQ(Projection::getMetersPerPixelAtLatitude(0, zoom), 78271.516964020484);
    EXPECT_DOUBLE_EQ(Projection::getMetersPerPixelAtLatitude(-util::LATITUDE_MAX, zoom), 6752.2284729446501);
    EXPECT_DOUBLE_EQ(Projection::getMetersPerPixelAtLatitude(util::LATITUDE_MAX, zoom),
                     Projection::getMetersPerPixelAtLatitude(-util::LATITUDE_MAX, zoom));

    zoom = 20;
    EXPECT_DOUBLE_EQ(Projection::getMetersPerPixelAtLatitude(0, zoom), 0.074645535434742435);
    EXPECT_DOUBLE_EQ(Projection::getMetersPerPixelAtLatitude(-util::LATITUDE_MAX, zoom), 0.0064394268731543065);

    EXPECT_DOUBLE_EQ(Projection::getMetersPerPixelAtLatitude(std::numeric_limits<double>::lowest(), zoom),
                     Projection::getMetersPerPixelAtLatitude(std::numeric_limits<double>::max(), zoom));

    zoom = std::numeric_limits<double>::min();
    EXPECT_DOUBLE_EQ(Projection::getMetersPerPixelAtLatitude(0, zoom),
                     Projection::getMetersPerPixelAtLatitude(0, util::MIN_ZOOM));

    zoom = std::numeric_limits<double>::lowest();
    EXPECT_DOUBLE_EQ(Projection::getMetersPerPixelAtLatitude(0, zoom),
                     Projection::getMetersPerPixelAtLatitude(0, util::MIN_ZOOM));

    zoom = std::numeric_limits<double>::max();
    EXPECT_DOUBLE_EQ(Projection::getMetersPerPixelAtLatitude(0, zoom),
                     Projection::getMetersPerPixelAtLatitude(0, util::MAX_ZOOM));
}

TEST(Projection, ProjectedMeters) {
    auto latLng = LatLng {};
    auto projectedMeters = Projection::projectedMetersForLatLng(latLng);
    EXPECT_EQ(projectedMeters.northing(), projectedMeters.easting());
    EXPECT_EQ(latLng, Projection::latLngForProjectedMeters(projectedMeters));

    const auto southWest = LatLng { -util::LATITUDE_MAX, -util::LONGITUDE_MAX };
    projectedMeters = Projection::projectedMetersForLatLng(southWest);
    EXPECT_DOUBLE_EQ(projectedMeters.northing(), -20037508.342789274);
    EXPECT_DOUBLE_EQ(projectedMeters.easting(), -20037508.342789244);

    const auto northEast = LatLng { util::LATITUDE_MAX, util::LONGITUDE_MAX };
    projectedMeters = Projection::projectedMetersForLatLng(northEast);
    EXPECT_DOUBLE_EQ(projectedMeters.northing(), 20037508.342789274);
    EXPECT_DOUBLE_EQ(projectedMeters.easting(), 20037508.342789244);

    projectedMeters = ProjectedMeters { std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest() };
    latLng = Projection::latLngForProjectedMeters(projectedMeters);
    EXPECT_EQ(latLng.latitude(), -util::LATITUDE_MAX);
    EXPECT_EQ(latLng.longitude(), -util::LONGITUDE_MAX);

    projectedMeters = ProjectedMeters { std::numeric_limits<double>::max(), std::numeric_limits<double>::max() };
    latLng = Projection::latLngForProjectedMeters(projectedMeters);
    EXPECT_EQ(latLng.latitude(), util::LATITUDE_MAX);
    EXPECT_EQ(latLng.longitude(), util::LONGITUDE_MAX);
}

TEST(Projection, InvalidProjectedMeters) {
    try {
        ProjectedMeters { NAN };
        ASSERT_TRUE(false) << "should throw";
    } catch (const std::domain_error& error) {
        ASSERT_EQ(std::string(error.what()), "northing must not be NaN");
    }
    try {
        ProjectedMeters { 0, NAN };
        ASSERT_TRUE(false) << "should throw";
    } catch (const std::domain_error& error) {
        ASSERT_EQ(std::string(error.what()), "easting must not be NaN");
    }
}
