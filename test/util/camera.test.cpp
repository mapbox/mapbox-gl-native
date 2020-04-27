#include <mbgl/test/util.hpp>

#include <mbgl/map/camera.hpp>
#include <mbgl/util/mat3.hpp>
#include <mbgl/util/quaternion.hpp>

#include <gmock/gmock.h>

using namespace mbgl;

static const double abs_double_error = 1e-7;

MATCHER_P(Vec3NearEquals, vec, "") {
    return std::fabs(vec[0] - arg[0]) <= abs_double_error && std::fabs(vec[1] - arg[1]) <= abs_double_error &&
           std::fabs(vec[2] - arg[2]) <= abs_double_error;
}

TEST(FreeCameraOptions, SetLocation) {
    FreeCameraOptions options;

    options.setLocation({{0.0, 0.0}, util::EARTH_RADIUS_M * M_PI});
    ASSERT_TRUE(options.position);
    ASSERT_THAT(options.position.value(), Vec3NearEquals(vec3{{0.5, 0.5, 0.5}}));

    options.setLocation({{25.0, -180.0}, 1000.0});
    ASSERT_TRUE(options.position);
    ASSERT_THAT(options.position.value(), Vec3NearEquals(vec3{{0.0, 0.4282409625, 0.000027532812465}}));

    options.setLocation(
        {{util::LATITUDE_MAX, 0.0}, util::EARTH_RADIUS_M * M_PI * std::cos(util::LATITUDE_MAX * util::DEG2RAD)});
    ASSERT_TRUE(options.position);
    ASSERT_THAT(options.position.value(), Vec3NearEquals(vec3{{0.5, 0.0, 0.5}}));

    options.setLocation(
        {{-util::LATITUDE_MAX, 0.0}, util::EARTH_RADIUS_M * M_PI * std::cos(-util::LATITUDE_MAX * util::DEG2RAD)});
    ASSERT_TRUE(options.position);
    ASSERT_THAT(options.position.value(), Vec3NearEquals(vec3{{0.5, 1.0, 0.5}}));
}

TEST(FreeCameraOptions, SetLocationNegativeAltitude) {
    FreeCameraOptions options;
    options.setLocation({{0.0, 0.0}, -100.0});
    ASSERT_TRUE(options.position);
    ASSERT_DOUBLE_EQ(options.position.value()[2], -100.0 / util::M2PI / util::EARTH_RADIUS_M);
}

TEST(FreeCameraOptions, SetLocationUnwrappedLocation) {
    FreeCameraOptions options;

    options.setLocation({{0.0, -540.0}, 0.0});
    ASSERT_TRUE(options.position);
    ASSERT_THAT(options.position.value(), Vec3NearEquals(vec3{{-1.0, 0.5, 0.0}}));
}

TEST(FreeCameraOptions, GetLocation) {
    FreeCameraOptions options;
    LatLngAltitude latLngAltitude;

    options.position = vec3{{0.5, 0.5, 0.5}};
    ASSERT_TRUE(options.getLocation());
    latLngAltitude = options.getLocation().value();
    ASSERT_DOUBLE_EQ(latLngAltitude.location.latitude(), 0.0);
    ASSERT_DOUBLE_EQ(latLngAltitude.location.longitude(), 0.0);
    ASSERT_DOUBLE_EQ(latLngAltitude.altitude, 20037508.342789244);

    options.position = vec3{{0.0, 0.4282409625, 0.000027532812465}};
    ASSERT_TRUE(options.getLocation());
    latLngAltitude = options.getLocation().value();
    ASSERT_NEAR(latLngAltitude.location.latitude(), 25.0, 1e-7);
    ASSERT_NEAR(latLngAltitude.location.longitude(), -180.0, 1e-7);
    ASSERT_NEAR(latLngAltitude.altitude, 1000.0, 1e-7);

    options.position = vec3{{0.5, 0.0, 0.5}};
    ASSERT_TRUE(options.getLocation());
    latLngAltitude = options.getLocation().value();
    ASSERT_NEAR(latLngAltitude.location.latitude(), util::LATITUDE_MAX, 1e-7);
    ASSERT_NEAR(latLngAltitude.location.longitude(), 0.0, 1e-7);
    ASSERT_NEAR(latLngAltitude.altitude, 1728570.489074, 1e-6);
}

TEST(FreeCameraOptions, GetLocationInvalidPosition) {
    FreeCameraOptions options;
    // Mercator position not set. Should return nothing
    ASSERT_FALSE(options.getLocation());

    // Invalid latitude
    options.position = vec3{{0.0, -0.1, 0.0}};
    ASSERT_FALSE(options.getLocation());
    options.position = vec3{{0.0, 2.0, 0.0}};
    ASSERT_FALSE(options.getLocation());
}

TEST(FreeCameraOptions, GetLocationNegativeAltitude) {
    FreeCameraOptions options;
    options.position = vec3{{0.5, 0.5, -0.25}};
    const auto latLngAltitude = options.getLocation();
    ASSERT_TRUE(latLngAltitude);

    ASSERT_DOUBLE_EQ(latLngAltitude->altitude, -0.5 * util::EARTH_RADIUS_M * M_PI);
}

TEST(FreeCameraOptions, GetLocationUnwrappedPosition) {
    FreeCameraOptions options;
    options.position = vec3{{1.25, 0.5, 0.0}};
    const auto latLngAltitude = options.getLocation();
    ASSERT_TRUE(latLngAltitude);
    ASSERT_DOUBLE_EQ(latLngAltitude->location.longitude(), 270.0);
    ASSERT_DOUBLE_EQ(latLngAltitude->location.latitude(), 0.0);
    ASSERT_DOUBLE_EQ(latLngAltitude->altitude, 0.0);
}

static std::tuple<vec3, vec3, vec3> rotatedFrame(const std::array<double, 4>& quaternion) {
    Quaternion q(quaternion);
    return std::make_tuple(
        q.transform({{1.0, 0.0, 0.0}}), q.transform({{0.0, -1.0, 0.0}}), q.transform({{0.0, 0.0, -1.0}}));
}

TEST(FreeCameraOptions, LookAtPoint) {
    FreeCameraOptions options;
    vec3 right, up, forward;
    const double cosPi4 = 1.0 / std::sqrt(2.0);

    // Pitch: 45, bearing: 0
    options.position = vec3{{0.5, 0.5, 0.5}};
    options.lookAtPoint({util::LATITUDE_MAX, 0.0});
    ASSERT_TRUE(options.orientation);
    std::tie(right, up, forward) = rotatedFrame(options.orientation.value());

    ASSERT_THAT(right, Vec3NearEquals(vec3{{1.0, 0.0, 0.0}}));
    ASSERT_THAT(up, Vec3NearEquals(vec3{{0.0, -cosPi4, cosPi4}}));
    ASSERT_THAT(forward, Vec3NearEquals(vec3{{0.0, -cosPi4, -cosPi4}}));

    // Look directly to east
    options.position = vec3{{0.5, 0.5, 0.0}};
    options.lookAtPoint({0.0, 30.0});
    ASSERT_TRUE(options.orientation);
    std::tie(right, up, forward) = rotatedFrame(options.orientation.value());

    ASSERT_THAT(right, Vec3NearEquals(vec3{{0.0, 1.0, 0.0}}));
    ASSERT_THAT(up, Vec3NearEquals(vec3{{0.0, 0.0, 1.0}}));
    ASSERT_THAT(forward, Vec3NearEquals(vec3{{1.0, 0.0, 0.0}}));

    // Pitch: 0, bearing: 0
    options.setLocation({{60.1699, 24.9384}, 100.0});
    options.lookAtPoint({60.1699, 24.9384}, vec3{{0.0, -1.0, 0.0}});
    ASSERT_TRUE(options.orientation);
    std::tie(right, up, forward) = rotatedFrame(options.orientation.value());

    ASSERT_THAT(right, Vec3NearEquals(vec3{{1.0, 0.0, 0.0}}));
    ASSERT_THAT(up, Vec3NearEquals(vec3{{0.0, -1.0, 0.0}}));
    ASSERT_THAT(forward, Vec3NearEquals(vec3{{0.0, 0.0, -1.0}}));

    // Pitch: 0, bearing: 45
    options.setLocation({{60.1699, 24.9384}, 100.0});
    options.lookAtPoint({60.1699, 24.9384}, vec3{{-1.0, -1.0, 0.0}});
    ASSERT_TRUE(options.orientation);
    std::tie(right, up, forward) = rotatedFrame(options.orientation.value());

    ASSERT_THAT(right, Vec3NearEquals(vec3{{cosPi4, -cosPi4, 0.0}}));
    ASSERT_THAT(up, Vec3NearEquals(vec3{{-cosPi4, -cosPi4, 0.0}}));
    ASSERT_THAT(forward, Vec3NearEquals(vec3{{0.0, 0.0, -1.0}}));

    // Looking south, up vector almost same as forward vector
    options.setLocation({{37.7749, 122.4194}, 0.0});
    options.lookAtPoint({37.5, 122.4194}, vec3{{0.0, 1.0, 0.00001}});
    ASSERT_TRUE(options.orientation);
    std::tie(right, up, forward) = rotatedFrame(options.orientation.value());

    ASSERT_THAT(right, Vec3NearEquals(vec3{{-1.0, 0.0, 0.0}}));
    ASSERT_THAT(up, Vec3NearEquals(vec3{{0.0, 0.0, 1.0}}));
    ASSERT_THAT(forward, Vec3NearEquals(vec3{{0.0, 1.0, 0.0}}));

    // Orientation with roll-component
    options.setLocation({{-33.8688, 151.2093}, 0.0});
    options.lookAtPoint({-33.8688, 160.0}, vec3{{0.0, -1.0, 0.1}});
    ASSERT_TRUE(options.orientation);
    std::tie(right, up, forward) = rotatedFrame(options.orientation.value());

    ASSERT_THAT(right, Vec3NearEquals(vec3{{0.0, 1.0, 0.0}}));
    ASSERT_THAT(up, Vec3NearEquals(vec3{{0.0, 0.0, 1.0}}));
    ASSERT_THAT(forward, Vec3NearEquals(vec3{{1.0, 0.0, 0.0}}));

    // Up vector pointing downwards
    options.position = vec3{{0.5, 0.5, 0.5}};
    options.lookAtPoint({util::LATITUDE_MAX, 0.0}, vec3{{0.0, 0.0, -0.5}});
    ASSERT_TRUE(options.orientation);
    std::tie(right, up, forward) = rotatedFrame(options.orientation.value());

    ASSERT_THAT(right, Vec3NearEquals(vec3{{1.0, 0.0, 0.0}}));
    ASSERT_THAT(up, Vec3NearEquals(vec3{{0.0, -cosPi4, cosPi4}}));
    ASSERT_THAT(forward, Vec3NearEquals(vec3{{0.0, -cosPi4, -cosPi4}}));
}

TEST(FreeCameraOptions, LookAtPointInvalidInput) {
    FreeCameraOptions options;

    // Position not set
    options.orientation = vec4{{0.0, 0.0, 0.0, 0.0}};
    options.lookAtPoint({0.0, 0.0});
    ASSERT_FALSE(options.orientation);

    // Target same as position
    options.orientation = vec4{{0.0, 0.0, 0.0, 0.0}};
    options.position = vec3{{0.5, 0.5, 0.0}};
    options.lookAtPoint({0.0, 0.0});
    ASSERT_FALSE(options.orientation);

    // Camera looking directly down without an explicit up vector
    options.orientation = vec4{{0.0, 0.0, 0.0, 0.0}};
    options.position = vec3{{0.5, 0.5, 0.5}};
    options.lookAtPoint({0.0, 0.0});
    ASSERT_FALSE(options.orientation);

    // Zero up vector
    options.orientation = vec4{{0.0, 0.0, 0.0, 0.0}};
    options.lookAtPoint({0.0, 0.0}, vec3{{0.0, 0.0, 0.0}});
    ASSERT_FALSE(options.orientation);

    // Up vector same as direction
    options.orientation = vec4{{0.0, 0.0, 0.0, 0.0}};
    options.lookAtPoint({0.0, 0.0}, vec3{{0.0, 0.0, -1.0}});
    ASSERT_FALSE(options.orientation);
}

TEST(FreeCameraOptions, SetPitchBearing) {
    FreeCameraOptions options;
    vec3 right, up, forward;

    options.setPitchBearing(0.0, 0.0);
    ASSERT_TRUE(options.orientation);
    std::tie(right, up, forward) = rotatedFrame(options.orientation.value());
    ASSERT_THAT(right, Vec3NearEquals(vec3{{1.0, 0.0, 0.0}}));
    ASSERT_THAT(up, Vec3NearEquals(vec3{{0.0, -1.0, 0.0}}));
    ASSERT_THAT(forward, Vec3NearEquals(vec3{{0.0, 0.0, -1.0}}));

    options.setPitchBearing(0.0, 180.0);
    ASSERT_TRUE(options.orientation);
    std::tie(right, up, forward) = rotatedFrame(options.orientation.value());
    ASSERT_THAT(right, Vec3NearEquals(vec3{{-1.0, 0.0, 0.0}}));
    ASSERT_THAT(up, Vec3NearEquals(vec3{{0.0, 1.0, 0.0}}));
    ASSERT_THAT(forward, Vec3NearEquals(vec3{{0.0, 0.0, -1.0}}));

    const double cos60 = std::cos(60.0 * util::DEG2RAD);
    const double sin60 = std::sin(60.0 * util::DEG2RAD);

    options.setPitchBearing(60.0, 0.0);
    ASSERT_TRUE(options.orientation);
    std::tie(right, up, forward) = rotatedFrame(options.orientation.value());
    ASSERT_THAT(right, Vec3NearEquals(vec3{{1.0, 0.0, 0.0}}));
    ASSERT_THAT(up, Vec3NearEquals(vec3{{0.0, -cos60, sin60}}));
    ASSERT_THAT(forward, Vec3NearEquals(vec3{{0.0, -sin60, -cos60}}));

    options.setPitchBearing(60.0, -450);
    ASSERT_TRUE(options.orientation);
    std::tie(right, up, forward) = rotatedFrame(options.orientation.value());
    ASSERT_THAT(right, Vec3NearEquals(vec3{{0.0, 1.0, 0.0}}));
    ASSERT_THAT(up, Vec3NearEquals(vec3{{cos60, 0.0, sin60}}));
    ASSERT_THAT(forward, Vec3NearEquals(vec3{{sin60, 0.0, -cos60}}));
}