#include "../fixtures/util.hpp"
#include "../fixtures/mock_view.hpp"

#include <mbgl/map/transform.hpp>
#include <mbgl/util/geo.hpp>

using namespace mbgl;

TEST(Transform, InvalidScale) {
    MockView view;
    Transform transform(view, ConstrainMode::HeightOnly);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(1, transform.getScale());

    transform.setScale(2 << 0);

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
    Transform transform(view, ConstrainMode::HeightOnly);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(1, transform.getScale());

    transform.setScale(2 << 0);
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
    Transform transform(view, ConstrainMode::HeightOnly);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(1, transform.getScale());

    transform.setScale(2 << 0);
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
    Transform transform(view, ConstrainMode::HeightOnly);

    transform.resize({{ 1000, 1000 }});
    transform.setScale(2 << 9);
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

    PrecisionPoint point = transform.getState().latLngToPoint({38.74661326302018, -77.59198961199148});
    ASSERT_NEAR(point.x, 0, 0.01);
    ASSERT_NEAR(point.y, 1000, 0.01);

    point = transform.getState().latLngToPoint({37.692872969426375, -76.75823239205641});
    ASSERT_NEAR(point.x, 1000, 0.02);
    ASSERT_NEAR(point.y, 0, 0.02);
}

TEST(Transform, ConstrainHeightOnly) {
    MockView view;
    LatLng loc;

    Transform transform(view, ConstrainMode::HeightOnly);
    transform.resize({{ 1000, 1000 }});
    transform.setScale(1024);

    transform.setLatLng(LatLngBounds::world().southwest());
    loc = transform.getState().pointToLatLng({ 500, 500 });
    ASSERT_NEAR(-85.021422866378714, loc.latitude, 0.0001);
    ASSERT_NEAR(180, std::abs(loc.longitude), 0.0001);

    transform.setLatLng(LatLngBounds::world().northeast());
    loc = transform.getState().pointToLatLng({ 500, 500 });
    ASSERT_NEAR(85.021422866378742, loc.latitude, 0.0001);
    ASSERT_NEAR(180, std::abs(loc.longitude), 0.0001);
}

TEST(Transform, ConstrainWidthAndHeight) {
    MockView view;
    LatLng loc;

    Transform transform(view, ConstrainMode::WidthAndHeight);
    transform.resize({{ 1000, 1000 }});
    transform.setScale(1024);

    transform.setLatLng(LatLngBounds::world().southwest());
    loc = transform.getState().pointToLatLng({ 500, 500 });
    ASSERT_NEAR(-85.021422866378714, loc.latitude, 0.0001);
    ASSERT_NEAR(-179.65667724609375, loc.longitude, 0.0001);

    transform.setLatLng(LatLngBounds::world().northeast());
    loc = transform.getState().pointToLatLng({ 500, 500 });
    ASSERT_NEAR(85.021422866378742, loc.latitude, 0.0001);
    ASSERT_NEAR(179.65667724609358, std::abs(loc.longitude), 0.0001);
}

TEST(Transform, Anchor) {
    MockView view;
    Transform transform(view, ConstrainMode::HeightOnly);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(1, transform.getScale());

    transform.setLatLngZoom({ 10, -100 }, 10);

    ASSERT_DOUBLE_EQ(10, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(-100, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(10, transform.getZoom());
    ASSERT_DOUBLE_EQ(0, transform.getAngle());

    const PrecisionPoint anchorPoint = {0, 0};
    const LatLng anchorLatLng = transform.getState().pointToLatLng(anchorPoint);
    transform.setAngle(M_PI_4, anchorPoint);

    ASSERT_NEAR(M_PI_4, transform.getAngle(), 0.000001);
    ASSERT_NE(anchorLatLng, transform.getLatLng());
    ASSERT_DOUBLE_EQ(anchorLatLng, transform.getState().pointToLatLng(anchorPoint));
}

TEST(Transform, Padding) {
    MockView view;
    Transform transform(view, ConstrainMode::HeightOnly);
    transform.resize({{ 1000, 1000 }});

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);

    transform.setLatLngZoom({ 10, -100 }, 10);

    const LatLng trueCenter = transform.getLatLng();
    ASSERT_DOUBLE_EQ(10, trueCenter.latitude);
    ASSERT_DOUBLE_EQ(-100, trueCenter.longitude);
    ASSERT_DOUBLE_EQ(10, transform.getZoom());
    
    const LatLng manualShiftedCenter = transform.getState().pointToLatLng({
        1000.0 / 2.0,
        1000.0 / 4.0,
    });
    
    EdgeInsets padding;
    padding.top = 1000.0 / 2.0;
    ASSERT_GT(padding.top, 0);
    ASSERT_TRUE(padding);
    
    const LatLng shiftedCenter = transform.getLatLng(padding);
    ASSERT_NE(trueCenter.latitude, shiftedCenter.latitude);
    ASSERT_DOUBLE_EQ(trueCenter.longitude, shiftedCenter.longitude);
    ASSERT_DOUBLE_EQ(manualShiftedCenter.latitude, shiftedCenter.latitude);
    ASSERT_DOUBLE_EQ(manualShiftedCenter.longitude, shiftedCenter.longitude);
}
