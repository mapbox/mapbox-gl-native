#include <mbgl/test/util.hpp>

#include <mbgl/map/transform.hpp>
#include <mbgl/util/geo.hpp>

using namespace mbgl;

TEST(Transform, InvalidScale) {
    Transform transform;

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

    transform.setLatLngZoom({0, 0}, invalid);

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());
}

TEST(Transform, InvalidLatLng) {
    Transform transform;

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(1, transform.getScale());

    transform.setScale(2 << 0);
    transform.setLatLng({8, 10});

    ASSERT_DOUBLE_EQ(8, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(10, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(2, transform.getScale());

    transform.setLatLngZoom({10, 8}, 2);

    ASSERT_DOUBLE_EQ(10, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(8, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(4, transform.getScale());

    const double invalid = std::nan("");
    transform.setLatLngZoom({invalid, 8}, 2);

    ASSERT_DOUBLE_EQ(10, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(8, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(4, transform.getScale());

    transform.setLatLngZoom({10, invalid}, 2);

    ASSERT_DOUBLE_EQ(10, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(8, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(4, transform.getScale());
}

TEST(Transform, InvalidBearing) {
    Transform transform;

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
    LatLng loc;

    Transform transform;
    transform.resize({{1000, 1000}});
    transform.setScale(2 << 9);
    transform.setPitch(0.9);
    transform.setLatLng(LatLng(38, -77));

    // expected values are from mapbox-gl-js

    loc = transform.getLatLng();
    ASSERT_NEAR(-77, loc.longitude, 0.0001);
    ASSERT_NEAR(38, loc.latitude, 0.0001);

    loc = transform.getState().screenCoordinateToLatLng({0, 1000});
    ASSERT_NEAR(-77.59198961199148, loc.longitude, 0.0002);
    ASSERT_NEAR(38.74661326302018, loc.latitude, 0.0001);

    loc = transform.getState().screenCoordinateToLatLng({1000, 0});
    ASSERT_NEAR(-76.75823239205641, loc.longitude, 0.0001);
    ASSERT_NEAR(37.692872969426375, loc.latitude, 0.0001);

    ScreenCoordinate point =
        transform.getState().latLngToScreenCoordinate({38.74661326302018, -77.59198961199148});
    ASSERT_NEAR(point.x, 0, 0.01);
    ASSERT_NEAR(point.y, 1000, 0.01);

    point = transform.getState().latLngToScreenCoordinate({37.692872969426375, -76.75823239205641});
    ASSERT_NEAR(point.x, 1000, 0.02);
    ASSERT_NEAR(point.y, 0, 0.02);
}

TEST(Transform, UnwrappedLatLng) {
    Transform transform;
    transform.resize({{1000, 1000}});
    transform.setScale(2 << 9);
    transform.setPitch(0.9);
    transform.setLatLng(LatLng(38, -77));

    const TransformState& state = transform.getState();

    LatLng fromGetLatLng = state.getLatLng();
    ASSERT_DOUBLE_EQ(fromGetLatLng.latitude, 38);
    ASSERT_DOUBLE_EQ(fromGetLatLng.longitude, -77);

    LatLng fromScreenCoordinate = state.screenCoordinateToLatLng({500, 500});
    ASSERT_NEAR(fromScreenCoordinate.latitude, 37.999999999999829, 0.0001);   // 1.71E-13
    ASSERT_NEAR(fromScreenCoordinate.longitude, -76.999999999999773, 0.0001); // 2.27E-13

    LatLng wrappedForwards =
        state.screenCoordinateToLatLng(state.latLngToScreenCoordinate({38, 283}));
    ASSERT_NEAR(wrappedForwards.latitude, 37.999999999999716, 0.0001);  // 2.84E-13
    ASSERT_NEAR(wrappedForwards.longitude, 282.99999999988751, 0.0001); // 1.1249E-11
    wrappedForwards.wrap();
    ASSERT_NEAR(wrappedForwards.longitude, -77.000000000112493, 0.001); // 1.1249E-11

    LatLng wrappedBackwards =
        state.screenCoordinateToLatLng(state.latLngToScreenCoordinate({38, -437}));
    ASSERT_NEAR(wrappedBackwards.latitude, wrappedForwards.latitude, 0.001);
    ASSERT_NEAR(wrappedBackwards.longitude, -436.99999999988728, 0.001); // 1.1272E-11
    wrappedBackwards.wrap();
    ASSERT_NEAR(wrappedBackwards.longitude, -76.99999999988728, 0.001); // 1.1272E-11
}

TEST(Transform, ConstrainHeightOnly) {
    LatLng loc;

    Transform transform;
    transform.resize({{1000, 1000}});
    transform.setScale(std::pow(2, util::MAX_ZOOM));

    transform.setLatLng(LatLngBounds::world().southwest());
    loc = transform.getLatLng();
    ASSERT_NEAR(-util::LATITUDE_MAX, loc.latitude, 0.001);
    ASSERT_NEAR(util::LONGITUDE_MAX, std::abs(loc.longitude), 0.001);

    transform.setLatLng(LatLngBounds::world().northeast());
    loc = transform.getLatLng();
    ASSERT_NEAR(util::LATITUDE_MAX, loc.latitude, 0.001);
    ASSERT_NEAR(util::LONGITUDE_MAX, std::abs(loc.longitude), 0.001);
}

TEST(Transform, ConstrainWidthAndHeight) {
    LatLng loc;

    Transform transform(nullptr, ConstrainMode::WidthAndHeight);
    transform.resize({{1000, 1000}});
    transform.setScale(std::pow(2, util::MAX_ZOOM));

    transform.setLatLng(LatLngBounds::world().southwest());
    loc = transform.getLatLng();
    ASSERT_NEAR(-util::LATITUDE_MAX, loc.latitude, 0.001);
    ASSERT_NEAR(-util::LONGITUDE_MAX, loc.longitude, 0.001);

    transform.setLatLng(LatLngBounds::world().northeast());
    loc = transform.getLatLng();
    ASSERT_NEAR(util::LATITUDE_MAX, loc.latitude, 0.001);
    ASSERT_NEAR(util::LONGITUDE_MAX, std::abs(loc.longitude), 0.001);
}

TEST(Transform, Anchor) {
    Transform transform;
    transform.resize({{1000, 1000}});

    const LatLng latLng{10, -100};
    transform.setLatLngZoom(latLng, 10);

    ASSERT_DOUBLE_EQ(latLng.latitude, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(latLng.longitude, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(10, transform.getZoom());
    ASSERT_DOUBLE_EQ(0, transform.getAngle());

    const optional<ScreenCoordinate> invalidAnchorPoint{};
    const ScreenCoordinate anchorPoint = {150, 150};

    const LatLng anchorLatLng = transform.getState().screenCoordinateToLatLng(anchorPoint);
    ASSERT_NE(latLng.latitude, anchorLatLng.latitude);
    ASSERT_NE(latLng.longitude, anchorLatLng.longitude);

    transform.setLatLngZoom(latLng, 2);
    transform.scaleBy(1);
    ASSERT_DOUBLE_EQ(4, transform.getScale());
    ASSERT_DOUBLE_EQ(latLng.latitude, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(latLng.longitude, transform.getLatLng().longitude);

    transform.scaleBy(1.5, invalidAnchorPoint);
    ASSERT_DOUBLE_EQ(6, transform.getScale());
    ASSERT_DOUBLE_EQ(latLng.latitude, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(latLng.longitude, transform.getLatLng().longitude);

    transform.scaleBy(2, anchorPoint);
    ASSERT_DOUBLE_EQ(12, transform.getScale());
    ASSERT_NE(latLng.latitude, transform.getLatLng().latitude);
    ASSERT_NE(latLng.longitude, transform.getLatLng().longitude);

    transform.setLatLngZoom(latLng, 10);
    transform.setScale(2 << 2);
    ASSERT_DOUBLE_EQ(2 << 2, transform.getScale());
    ASSERT_NEAR(latLng.latitude, transform.getLatLng().latitude, 0.000001);
    ASSERT_NEAR(latLng.longitude, transform.getLatLng().longitude, 0.000001);

    transform.setScale(2 << 4, invalidAnchorPoint);
    ASSERT_DOUBLE_EQ(2 << 4, transform.getScale());
    ASSERT_NEAR(latLng.latitude, transform.getLatLng().latitude, 0.000001);
    ASSERT_NEAR(latLng.longitude, transform.getLatLng().longitude, 0.000001);

    transform.setScale(2 << 6, anchorPoint);
    ASSERT_DOUBLE_EQ(2 << 6, transform.getScale());
    ASSERT_NE(latLng.latitude, transform.getLatLng().latitude);
    ASSERT_NE(latLng.longitude, transform.getLatLng().longitude);

    transform.setLatLngZoom(latLng, 10);
    transform.setZoom(2);
    ASSERT_DOUBLE_EQ(2, transform.getZoom());
    ASSERT_NEAR(latLng.latitude, transform.getLatLng().latitude, 0.000001);
    ASSERT_NEAR(latLng.longitude, transform.getLatLng().longitude, 0.000001);

    transform.setZoom(4, invalidAnchorPoint);
    ASSERT_DOUBLE_EQ(4, transform.getZoom());
    ASSERT_NEAR(latLng.latitude, transform.getLatLng().latitude, 0.000001);
    ASSERT_NEAR(latLng.longitude, transform.getLatLng().longitude, 0.000001);

    transform.setZoom(8, anchorPoint);
    ASSERT_DOUBLE_EQ(8, transform.getZoom());
    ASSERT_NE(latLng.latitude, transform.getLatLng().latitude);
    ASSERT_NE(latLng.longitude, transform.getLatLng().longitude);

    transform.setLatLngZoom(latLng, 10);
    transform.setAngle(M_PI_4);
    ASSERT_NEAR(M_PI_4, transform.getAngle(), 0.000001);
    ASSERT_DOUBLE_EQ(latLng.latitude, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(latLng.longitude, transform.getLatLng().longitude);

    transform.setAngle(0, invalidAnchorPoint);
    ASSERT_DOUBLE_EQ(0, transform.getAngle());
    ASSERT_DOUBLE_EQ(latLng.latitude, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(latLng.longitude, transform.getLatLng().longitude);

    transform.setAngle(45 * util::DEG2RAD, anchorPoint);
    ASSERT_NEAR(45 / util::RAD2DEG, transform.getAngle(), 0.000001);
    ASSERT_NEAR(anchorLatLng.latitude, transform.getLatLng().latitude, 1);
    ASSERT_NEAR(anchorLatLng.longitude, transform.getLatLng().longitude, 1);

    transform.setLatLngZoom(latLng, 10);
    transform.setPitch(10 * util::DEG2RAD);
    ASSERT_DOUBLE_EQ(10 / util::RAD2DEG, transform.getPitch());
    ASSERT_DOUBLE_EQ(latLng.latitude, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(latLng.longitude, transform.getLatLng().longitude);

    transform.setPitch(15 * util::DEG2RAD, invalidAnchorPoint);
    ASSERT_DOUBLE_EQ(15 / util::RAD2DEG, transform.getPitch());
    ASSERT_DOUBLE_EQ(latLng.latitude, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(latLng.longitude, transform.getLatLng().longitude);

    transform.setPitch(20 * util::DEG2RAD, anchorPoint);
    ASSERT_DOUBLE_EQ(20 / util::RAD2DEG, transform.getPitch());
    ASSERT_NEAR(anchorLatLng.latitude, transform.getLatLng().latitude, 1);
    ASSERT_NEAR(anchorLatLng.longitude, transform.getLatLng().longitude, 1);
}

TEST(Transform, Padding) {
    Transform transform;
    transform.resize({{1000, 1000}});

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude);

    transform.setLatLngZoom({10, -100}, 10);

    const LatLng trueCenter = transform.getLatLng();
    ASSERT_DOUBLE_EQ(10, trueCenter.latitude);
    ASSERT_DOUBLE_EQ(-100, trueCenter.longitude);
    ASSERT_DOUBLE_EQ(10, transform.getZoom());

    const LatLng manualShiftedCenter = transform.getState().screenCoordinateToLatLng({
        1000.0 / 2.0, 1000.0 / 4.0,
    });

    EdgeInsets padding;

    padding.top = 0;
    ASSERT_FALSE(bool(padding));

    padding.top = NAN;
    ASSERT_FALSE(bool(padding));

    padding.top = 1000.0 / 2.0;
    ASSERT_TRUE(bool(padding));

    const LatLng shiftedCenter = transform.getLatLng(padding);
    ASSERT_NE(trueCenter.latitude, shiftedCenter.latitude);
    ASSERT_DOUBLE_EQ(trueCenter.longitude, shiftedCenter.longitude);
    ASSERT_DOUBLE_EQ(manualShiftedCenter.latitude, shiftedCenter.latitude);
    ASSERT_DOUBLE_EQ(manualShiftedCenter.longitude, shiftedCenter.longitude);
}

TEST(Transform, MoveBy) {
    Transform transform;
    transform.resize({{1000, 1000}});
    transform.setLatLngZoom({0, 0}, 10);

    LatLng trueCenter = transform.getLatLng();
    ASSERT_DOUBLE_EQ(0, trueCenter.latitude);
    ASSERT_DOUBLE_EQ(0, trueCenter.longitude);
    ASSERT_DOUBLE_EQ(10, transform.getZoom());

    for (uint8_t x = 0; x < 20; ++x) {
        bool odd = x % 2;
        bool forward = x % 10;

        LatLng coordinate =
            transform.screenCoordinateToLatLng({odd ? 400. : 600., forward ? 400. : 600});
        transform.moveBy({odd ? 100. : -100., forward ? 100. : -100});

        trueCenter = transform.getLatLng();
        ASSERT_NEAR(coordinate.latitude, trueCenter.latitude, 0.0001);
        ASSERT_NEAR(coordinate.longitude, trueCenter.longitude, 0.0001);
    }

    // We have ~1.1 precision loss for each coordinate for 20 rounds of moveBy.
    ASSERT_NEAR(0, trueCenter.latitude, 1.1);
    ASSERT_NEAR(0, trueCenter.longitude, 1.1);
}

TEST(Transform, Antimeridian) {
    Transform transform;
    transform.resize({{1000, 1000}});
    transform.setLatLngZoom({0, 0}, 1);

    const LatLng coordinateSanFrancisco{37.7833, -122.4167};
    ScreenCoordinate pixelSF = transform.latLngToScreenCoordinate(coordinateSanFrancisco);
    ASSERT_DOUBLE_EQ(151.79409149185352, pixelSF.x);
    ASSERT_DOUBLE_EQ(383.76774094913071, pixelSF.y);

    transform.setLatLng({0, -181});
    ScreenCoordinate pixelSFBackwards = transform.latLngToScreenCoordinate(coordinateSanFrancisco);
    ASSERT_DOUBLE_EQ(666.63617954008976, pixelSFBackwards.x);
    ASSERT_DOUBLE_EQ(pixelSF.y, pixelSFBackwards.y);

    transform.setLatLng({0, 179});
    ScreenCoordinate pixelSFForwards = transform.latLngToScreenCoordinate(coordinateSanFrancisco);
    ASSERT_DOUBLE_EQ(pixelSFBackwards.x, pixelSFForwards.x);
    ASSERT_DOUBLE_EQ(pixelSFBackwards.y, pixelSFForwards.y);

    const LatLng coordinateWaikiri{-16.9310, 179.9787};
    transform.setLatLngZoom(coordinateWaikiri, 10);
    ScreenCoordinate pixelWaikiri = transform.latLngToScreenCoordinate(coordinateWaikiri);
    ASSERT_DOUBLE_EQ(500.00000000007759, pixelWaikiri.x);
    ASSERT_DOUBLE_EQ(500, pixelWaikiri.y);

    transform.setLatLng({coordinateWaikiri.latitude, 180.0213});
    ScreenCoordinate pixelWaikiriForwards = transform.latLngToScreenCoordinate(coordinateWaikiri);
    ASSERT_DOUBLE_EQ(437.95953728819512, pixelWaikiriForwards.x);
    ASSERT_DOUBLE_EQ(pixelWaikiri.y, pixelWaikiriForwards.y);
    LatLng coordinateFromPixel = transform.screenCoordinateToLatLng(pixelWaikiriForwards);
    ASSERT_NEAR(coordinateWaikiri.latitude, coordinateFromPixel.latitude, 0.000001);
    ASSERT_NEAR(coordinateWaikiri.longitude, coordinateFromPixel.longitude, 0.000001);

    transform.setLatLng({coordinateWaikiri.latitude, -179.9787});
    ScreenCoordinate pixelWaikiriBackwards = transform.latLngToScreenCoordinate(coordinateWaikiri);
    ASSERT_DOUBLE_EQ(pixelWaikiriForwards.x, pixelWaikiriBackwards.x);
    ASSERT_DOUBLE_EQ(pixelWaikiriForwards.y, pixelWaikiriBackwards.y);
    coordinateFromPixel = transform.screenCoordinateToLatLng(pixelWaikiriBackwards);
    ASSERT_NEAR(coordinateWaikiri.latitude, coordinateFromPixel.latitude, 0.000001);
    ASSERT_NEAR(coordinateWaikiri.longitude, coordinateFromPixel.longitude, 0.000001);
}

TEST(Transform, Camera) {
    Transform transform;
    transform.resize({{1000, 1000}});

    const LatLng latLng1{45, 135};
    CameraOptions cameraOptions1;
    cameraOptions1.zoom = 20;
    cameraOptions1.center = latLng1;
    transform.jumpTo(cameraOptions1);

    ASSERT_DOUBLE_EQ(latLng1.latitude, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(latLng1.longitude, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(20, transform.getZoom());

    const LatLng latLng2{-45, -135};
    CameraOptions cameraOptions2;
    cameraOptions2.zoom = 10;
    cameraOptions2.center = latLng2;

    transform.jumpTo(cameraOptions2);
    ASSERT_DOUBLE_EQ(latLng2.latitude, transform.getLatLng().latitude);
    ASSERT_DOUBLE_EQ(latLng2.longitude, transform.getLatLng().longitude);
    ASSERT_DOUBLE_EQ(10, transform.getZoom());

    AnimationOptions easeOptions(Seconds(1));
    easeOptions.transitionFrameFn = [&](double t) {
        ASSERT_TRUE(t >= 0 && t <= 1);
        ASSERT_GE(latLng1.latitude, transform.getLatLng().latitude);
        ASSERT_LE(latLng1.longitude, transform.getLatLng().longitude);
    };
    easeOptions.transitionFinishFn = [&]() {
        ASSERT_NEAR(latLng1.latitude, transform.getLatLng().latitude, 0.000001);
        ASSERT_NEAR(latLng1.longitude, transform.getLatLng().longitude, 0.000001);
        ASSERT_NEAR(20, transform.getZoom(), 0.000001);
    };

    transform.easeTo(cameraOptions1, easeOptions);
    ASSERT_TRUE(transform.inTransition());
    transform.updateTransitions(transform.getTransitionStart() + Milliseconds(250));
    transform.updateTransitions(transform.getTransitionStart() + Milliseconds(500));
    transform.updateTransitions(transform.getTransitionStart() + Milliseconds(750));
    transform.updateTransitions(transform.getTransitionStart() + transform.getTransitionDuration());
    ASSERT_FALSE(transform.inTransition());

    AnimationOptions flyOptions(Seconds(1));
    flyOptions.transitionFrameFn = [&](double t) {
        ASSERT_TRUE(t >= 0 && t <= 1);
        ASSERT_LE(latLng2.latitude, transform.getLatLng().latitude);
        ASSERT_GE(latLng2.longitude, transform.getLatLng().longitude);
    };
    flyOptions.transitionFinishFn = [&]() {
        // XXX Fix precision loss in flyTo:
        // https://github.com/mapbox/mapbox-gl-native/issues/4298
        ASSERT_NEAR(latLng2.latitude, transform.getLatLng().latitude, 0.001);
        ASSERT_NEAR(latLng2.longitude, transform.getLatLng().longitude, 0.001);
        ASSERT_NEAR(10, transform.getZoom(), 0.00001);
    };

    transform.flyTo(cameraOptions2, flyOptions);
    ASSERT_TRUE(transform.inTransition());
    transform.updateTransitions(transform.getTransitionStart() + Milliseconds(250));
    transform.updateTransitions(transform.getTransitionStart() + Milliseconds(500));
    transform.updateTransitions(transform.getTransitionStart() + Milliseconds(750));
    transform.updateTransitions(transform.getTransitionStart() + transform.getTransitionDuration());
    ASSERT_FALSE(transform.inTransition());
}

TEST(Transform, DefaultTransform) {
    Transform transform;
    const TransformState& state = transform.getState();

    LatLng nullIsland, latLng = {};
    ScreenCoordinate center, point = {};
    const uint16_t min = std::numeric_limits<uint16_t>::min();
    const uint16_t max = std::numeric_limits<uint16_t>::max();

    auto testConversions = [&](const LatLng& coord, const ScreenCoordinate& screenCoord) {
        latLng = state.screenCoordinateToLatLng(center);
        ASSERT_NEAR(latLng.latitude, coord.latitude, 0.000001);
        ASSERT_NEAR(latLng.longitude, coord.longitude, 0.000001);
        point = state.latLngToScreenCoordinate(nullIsland);
        ASSERT_DOUBLE_EQ(point.x, screenCoord.x);
        ASSERT_DOUBLE_EQ(point.y, screenCoord.y);
    };

    testConversions(nullIsland, center);

    // Cannot assign the current size.
    ASSERT_FALSE(transform.resize({{}}));

    ASSERT_TRUE(transform.resize({{min, max}}));
    testConversions(nullIsland, center);

    ASSERT_TRUE(transform.resize({{max, min}}));
    testConversions(nullIsland, center);

    ASSERT_TRUE(transform.resize({{min, min}}));
    testConversions(nullIsland, center);

    center = {max / 2., max / 2.};

    // -1 evaluates to UINT_MAX.
    ASSERT_TRUE(transform.resize({{static_cast<uint16_t>(-1), static_cast<uint16_t>(-1)}}));
    ASSERT_FALSE(transform.resize({{max, max}}));
    testConversions(nullIsland, center);
}
