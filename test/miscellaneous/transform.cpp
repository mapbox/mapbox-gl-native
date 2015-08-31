#include "../fixtures/util.hpp"
#include "../fixtures/mock_view.hpp"

#include <mbgl/map/transform.hpp>

using namespace mbgl;

TEST(Transform, InvalidScale) {
    MockView view;
    Transform transform(view);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(1, transform.getScale());

    transform.setScale(2);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());

    const double invalid = std::nan("");
    transform.setScale(invalid);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());

    transform.scaleBy(invalid);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());

    transform.setZoom(invalid);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());

    transform.setLatLngZoom({ 0, 0 }, invalid);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());
}

TEST(Transform, InvalidLatLng) {
    MockView view;
    Transform transform(view);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(1, transform.getScale());

    transform.setScale(2);
    transform.setLatLng({ 8, 10 });

    ASSERT_DOUBLE_EQ(8, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(10, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());

    transform.setLatLngZoom({ 10, 8 }, 2);

    ASSERT_DOUBLE_EQ(10, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(8, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(4, transform.getScale());

    const double invalid = std::nan("");
    transform.setLatLngZoom({ invalid, 8 }, 2);

    ASSERT_DOUBLE_EQ(10, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(8, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(4, transform.getScale());

    transform.setLatLngZoom({ 10, invalid }, 2);

    ASSERT_DOUBLE_EQ(10, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(8, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(4, transform.getScale());
}


TEST(Transform, InvalidBearing) {
    MockView view;
    Transform transform(view);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(1, transform.getScale());

    transform.setScale(2);
    transform.setAngle(2);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());
    ASSERT_DOUBLE_EQ(2, transform.getAngle());

    const double invalid = std::nan("");
    transform.setAngle(invalid);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());
    ASSERT_DOUBLE_EQ(2, transform.getAngle());
}

TEST(Transform, PerspectiveProjection) {
    MockView view;
    Transform transform(view);

    transform.resize({{ 1000, 1000 }});
    transform.setScale(1024);
    transform.setPitch(0.9);
    transform.setLatLng(LatLng(38, -77));

    // expected values are from mapbox-gl-js

    LatLng loc = transform.getState().pointToLatLng({ 500, 500 });
    ASSERT_NEAR(-77, loc.longitude, 0.0001);
    ASSERT_NEAR(38, loc.latitude, 0.0001);

    loc = transform.getState().pointToLatLng({ 0, 1000 });
    ASSERT_NEAR(-77.59198961199148, loc.longitude, 0.0002);
    ASSERT_NEAR(38.74661326302018, loc.latitude, 0.0001);

    loc = transform.getState().pointToLatLng({ 1000, 0 });
    ASSERT_NEAR(-76.75823239205641, loc.longitude, 0.0001);
    ASSERT_NEAR(37.692872969426375, loc.latitude, 0.0001);

    vec2<double> point = transform.getState().latLngToPoint({38.74661326302018, -77.59198961199148});
    ASSERT_NEAR(point.x, 0, 0.01);
    ASSERT_NEAR(point.y, 1000, 0.01);

    point = transform.getState().latLngToPoint({37.692872969426375, -76.75823239205641});
    ASSERT_NEAR(point.x, 1000, 0.02);
    ASSERT_NEAR(point.y, 0, 0.02);
}
