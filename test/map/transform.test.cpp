#include <mbgl/test/util.hpp>

#include <mbgl/map/transform.hpp>
#include <mbgl/util/geo.hpp>

#include <cmath>

using namespace mbgl;

TEST(Transform, InvalidZoom) {
    Transform transform;

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude());
    ASSERT_DOUBLE_EQ(0, transform.getZoom());

    transform.jumpTo(CameraOptions().withZoom(1.0));

    ASSERT_DOUBLE_EQ(1, transform.getZoom());

    const double invalid = NAN;

    transform.jumpTo(CameraOptions().withZoom(invalid));

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude());
    ASSERT_DOUBLE_EQ(1, transform.getZoom());

    transform.jumpTo(CameraOptions().withCenter(LatLng()).withZoom(invalid));

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude());
    ASSERT_DOUBLE_EQ(1, transform.getZoom());

    transform.jumpTo(CameraOptions().withZoom(transform.getState().getMaxZoom() + 0.1));
    ASSERT_DOUBLE_EQ(transform.getZoom(), transform.getState().getMaxZoom());

    // Executing flyTo with an empty size causes frameZoom to be NaN.
    transform.flyTo(CameraOptions()
                        .withCenter(LatLng{ util::LATITUDE_MAX, util::LONGITUDE_MAX })
                        .withZoom(transform.getState().getMaxZoom()));
    transform.updateTransitions(transform.getTransitionStart() + transform.getTransitionDuration());
    ASSERT_DOUBLE_EQ(transform.getZoom(), transform.getState().getMaxZoom());

    // Executing flyTo with maximum zoom level to the same zoom level causes
    // frameZoom to be bigger than maximum zoom.
    transform.resize(Size { 100, 100 });
    transform.flyTo(CameraOptions()
                        .withCenter(LatLng{ util::LATITUDE_MAX, util::LONGITUDE_MAX })
                        .withZoom(transform.getState().getMaxZoom()));
    transform.updateTransitions(transform.getTransitionStart() + transform.getTransitionDuration());

    ASSERT_TRUE(transform.getState().valid());
    ASSERT_DOUBLE_EQ(transform.getState().getMaxZoom(), transform.getZoom());
}

TEST(Transform, InvalidBearing) {
    Transform transform;

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude());
    ASSERT_DOUBLE_EQ(0, transform.getZoom());

    transform.jumpTo(CameraOptions().withZoom(1.0).withAngle(2.0));
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude());
    ASSERT_DOUBLE_EQ(1, transform.getZoom());
    ASSERT_NEAR(-2.0 * util::DEG2RAD, transform.getAngle(), 1e-15);

    const double invalid = NAN;

    transform.jumpTo(CameraOptions().withAngle(invalid));
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude());
    ASSERT_DOUBLE_EQ(1, transform.getZoom());
    ASSERT_NEAR(-2.0 * util::DEG2RAD, transform.getAngle(), 1e-15);
}

TEST(Transform, IntegerZoom) {
    Transform transform;

    auto checkIntegerZoom = [&transform](uint8_t zoomInt, double zoom) {
        transform.jumpTo(CameraOptions().withZoom(zoom));
        ASSERT_NEAR(transform.getZoom(), zoom, 0.0001);
        ASSERT_EQ(transform.getState().getIntegerZoom(), zoomInt);
        ASSERT_NEAR(transform.getState().getZoomFraction(), zoom - zoomInt, 0.0001);
    };

    for (uint8_t zoomInt = 0; zoomInt < 20; ++zoomInt) {
        for (uint32_t percent = 0; percent < 100; ++percent) {
            double zoom = zoomInt + (0.01 * percent);
            checkIntegerZoom(zoomInt, zoom);
        }
    }

    // Special case zoom 20.
    checkIntegerZoom(20, 20.0);
}

TEST(Transform, PerspectiveProjection) {
    LatLng loc;

    Transform transform;
    transform.resize({ 1000, 1000 });

    // 0.9 rad ~ 51.5662 deg
    transform.jumpTo(CameraOptions().withCenter(LatLng { 38.0, -77.0 }).withZoom(10.0).withPitch(51.5662));

    // expected values are from mapbox-gl-js

    loc = transform.getLatLng();
    ASSERT_NEAR(-77, loc.longitude(), 0.0001);
    ASSERT_NEAR(38, loc.latitude(), 0.0001);

    loc = transform.getState().screenCoordinateToLatLng({ 0, 1000 });
    ASSERT_NEAR(-77.59198961199148, loc.longitude(), 0.0002);
    ASSERT_NEAR(38.74661326302018, loc.latitude(), 0.0001);

    loc = transform.getState().screenCoordinateToLatLng({ 1000, 0 });
    ASSERT_NEAR(-76.75823239205641, loc.longitude(), 0.0001);
    ASSERT_NEAR(37.692872969426375, loc.latitude(), 0.0001);

    ScreenCoordinate point = transform.getState().latLngToScreenCoordinate({38.74661326302018, -77.59198961199148});
    ASSERT_NEAR(point.x, 0, 0.01);
    ASSERT_NEAR(point.y, 1000, 0.01);

    point = transform.getState().latLngToScreenCoordinate({37.692872969426375, -76.75823239205641});
    ASSERT_NEAR(point.x, 1000, 0.02);
    ASSERT_NEAR(point.y, 0, 0.02);
}

TEST(Transform, UnwrappedLatLng) {
    Transform transform;
    transform.resize({ 1000, 1000 });

    // 0.9 rad ~ 51.5662 deg
    transform.jumpTo(CameraOptions().withCenter(LatLng { 38.0, -77.0 }).withZoom(10.0).withPitch(51.5662));

    const TransformState& state = transform.getState();

    LatLng fromGetLatLng = state.getLatLng();
    ASSERT_DOUBLE_EQ(fromGetLatLng.latitude(), 38);
    ASSERT_DOUBLE_EQ(fromGetLatLng.longitude(), -77);

    LatLng fromScreenCoordinate = state.screenCoordinateToLatLng({ 500, 500 });
    ASSERT_NEAR(fromScreenCoordinate.latitude(),   37.999999999999829, 0.0001); // 1.71E-13
    ASSERT_NEAR(fromScreenCoordinate.longitude(), -76.999999999999773, 0.0001); // 2.27E-13

    LatLng wrappedRightwards = state.screenCoordinateToLatLng(state.latLngToScreenCoordinate({ 38, 283 }));
    ASSERT_NEAR(wrappedRightwards.latitude(), 37.999999999999716, 0.0001); // 2.84E-13
    ASSERT_NEAR(wrappedRightwards.longitude(), 282.99999999988751, 0.0001); // 1.1249E-11
    wrappedRightwards.wrap();
    ASSERT_NEAR(wrappedRightwards.longitude(), -77.000000000112493, 0.001); // 1.1249E-11

    LatLng wrappedLeftwards = state.screenCoordinateToLatLng(state.latLngToScreenCoordinate({ 38, -437 }));
    ASSERT_NEAR(wrappedLeftwards.latitude(), wrappedRightwards.latitude(), 0.001);
    ASSERT_NEAR(wrappedLeftwards.longitude(), -436.99999999988728, 0.001); // 1.1272E-11
    wrappedLeftwards.wrap();
    ASSERT_NEAR(wrappedLeftwards.longitude(), -76.99999999988728, 0.001); // 1.1272E-11
}

TEST(Transform, ConstrainHeightOnly) {
    Transform transform;
    transform.resize({ 1000, 1000 });

    transform.jumpTo(CameraOptions().withCenter(LatLngBounds::world().southwest()).withZoom(util::MAX_ZOOM));
    ASSERT_NEAR(-util::LATITUDE_MAX, transform.getLatLng().latitude(), 0.001);
    ASSERT_NEAR(util::LONGITUDE_MAX, std::abs(transform.getLatLng().longitude()), 0.001);

    transform.jumpTo(CameraOptions().withCenter(LatLngBounds::world().northeast()));
    ASSERT_NEAR(util::LATITUDE_MAX, transform.getLatLng().latitude(), 0.001);
    ASSERT_NEAR(util::LONGITUDE_MAX, std::abs(transform.getLatLng().longitude()), 0.001);
}

TEST(Transform, ConstrainWidthAndHeight) {
    Transform transform(MapObserver::nullObserver(), ConstrainMode::WidthAndHeight);
    transform.resize({ 1000, 1000 });

    transform.jumpTo(CameraOptions().withCenter(LatLngBounds::world().southwest()).withZoom(util::MAX_ZOOM));
    ASSERT_NEAR(-util::LATITUDE_MAX, transform.getLatLng().latitude(), 0.001);
    ASSERT_NEAR(-util::LONGITUDE_MAX, transform.getLatLng().longitude(), 0.001);

    transform.jumpTo(CameraOptions().withCenter(LatLngBounds::world().northeast()));
    ASSERT_NEAR(util::LATITUDE_MAX, transform.getLatLng().latitude(), 0.001);
    ASSERT_NEAR(util::LONGITUDE_MAX, std::abs(transform.getLatLng().longitude()), 0.001);
}

TEST(Transform, Anchor) {
    Transform transform;
    transform.resize({ 1000, 1000 });

    const LatLng latLng { 10, -100 };
    const ScreenCoordinate anchorPoint = { 150, 150 };

    transform.jumpTo(CameraOptions().withCenter(latLng).withZoom(10.0));
    ASSERT_DOUBLE_EQ(latLng.latitude(), transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(latLng.longitude(), transform.getLatLng().longitude());
    ASSERT_DOUBLE_EQ(10, transform.getZoom());
    ASSERT_DOUBLE_EQ(0, transform.getAngle());

    const LatLng anchorLatLng = transform.getState().screenCoordinateToLatLng(anchorPoint);
    ASSERT_NE(latLng.latitude(), anchorLatLng.latitude());
    ASSERT_NE(latLng.longitude(), anchorLatLng.longitude());

    transform.jumpTo(CameraOptions().withCenter(latLng).withZoom(3.0));
    ASSERT_DOUBLE_EQ(3, transform.getZoom());
    ASSERT_DOUBLE_EQ(latLng.latitude(), transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(latLng.longitude(), transform.getLatLng().longitude());

    transform.jumpTo(CameraOptions().withZoom(3.5));
    ASSERT_DOUBLE_EQ(3.5, transform.getZoom());
    ASSERT_DOUBLE_EQ(latLng.latitude(), transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(latLng.longitude(), transform.getLatLng().longitude());

    transform.jumpTo(CameraOptions().withZoom(5.5).withAnchor(anchorPoint));
    ASSERT_DOUBLE_EQ(5.5, transform.getZoom());
    ASSERT_NE(latLng.latitude(), transform.getLatLng().latitude());
    ASSERT_NE(latLng.longitude(), transform.getLatLng().longitude());

    transform.jumpTo(CameraOptions().withCenter(latLng).withZoom(3.0));
    ASSERT_DOUBLE_EQ(3, transform.getZoom());
    ASSERT_NEAR(latLng.latitude(), transform.getLatLng().latitude(), 0.000001);
    ASSERT_NEAR(latLng.longitude(), transform.getLatLng().longitude(), 0.000001);

    transform.jumpTo(CameraOptions().withZoom(5.0));
    ASSERT_DOUBLE_EQ(5, transform.getZoom());
    ASSERT_NEAR(latLng.latitude(), transform.getLatLng().latitude(), 0.000001);
    ASSERT_NEAR(latLng.longitude(), transform.getLatLng().longitude(), 0.000001);

    transform.jumpTo(CameraOptions().withZoom(7.0).withAnchor(anchorPoint));
    ASSERT_DOUBLE_EQ(7, transform.getZoom());
    ASSERT_NE(latLng.latitude(), transform.getLatLng().latitude());
    ASSERT_NE(latLng.longitude(), transform.getLatLng().longitude());

    transform.jumpTo(CameraOptions().withCenter(latLng).withZoom(2.0));
    ASSERT_DOUBLE_EQ(2, transform.getZoom());
    ASSERT_NEAR(latLng.latitude(), transform.getLatLng().latitude(), 0.000001);
    ASSERT_NEAR(latLng.longitude(), transform.getLatLng().longitude(), 0.000001);

    transform.jumpTo(CameraOptions().withZoom(4.0));
    ASSERT_DOUBLE_EQ(4, transform.getZoom());
    ASSERT_NEAR(latLng.latitude(), transform.getLatLng().latitude(), 0.000001);
    ASSERT_NEAR(latLng.longitude(), transform.getLatLng().longitude(), 0.000001);

    transform.jumpTo(CameraOptions().withZoom(8.0).withAnchor(anchorPoint));
    ASSERT_DOUBLE_EQ(8, transform.getZoom());
    ASSERT_NE(latLng.latitude(), transform.getLatLng().latitude());
    ASSERT_NE(latLng.longitude(), transform.getLatLng().longitude());

    transform.jumpTo(CameraOptions().withCenter(latLng).withZoom(10.0).withAngle(-45.0));
    ASSERT_NEAR(M_PI_4, transform.getAngle(), 0.000001);
    ASSERT_DOUBLE_EQ(latLng.latitude(), transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(latLng.longitude(), transform.getLatLng().longitude());

    transform.jumpTo(CameraOptions().withAngle(0.0));
    ASSERT_DOUBLE_EQ(0, transform.getAngle());
    ASSERT_DOUBLE_EQ(latLng.latitude(), transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(latLng.longitude(), transform.getLatLng().longitude());

    transform.jumpTo(CameraOptions().withAngle(45.0).withAnchor(anchorPoint));
    ASSERT_NEAR(-45.0 * util::DEG2RAD, transform.getAngle(), 0.000001);
    ASSERT_NEAR(anchorLatLng.latitude(), transform.getLatLng().latitude(), 1);
    ASSERT_NEAR(anchorLatLng.longitude(), transform.getLatLng().longitude(), 1);

    transform.jumpTo(CameraOptions().withCenter(latLng).withZoom(10.0).withPitch(10.0));
    ASSERT_DOUBLE_EQ(10.0 * util::DEG2RAD, transform.getPitch());
    ASSERT_DOUBLE_EQ(latLng.latitude(), transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(latLng.longitude(), transform.getLatLng().longitude());

    transform.jumpTo(CameraOptions().withPitch(15.0));
    ASSERT_DOUBLE_EQ(15.0 * util::DEG2RAD, transform.getPitch());
    ASSERT_DOUBLE_EQ(latLng.latitude(), transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(latLng.longitude(), transform.getLatLng().longitude());

    transform.jumpTo(CameraOptions().withPitch(20.0).withAnchor(anchorPoint));
    ASSERT_DOUBLE_EQ(20.0 * util::DEG2RAD, transform.getPitch());
    ASSERT_NEAR(anchorLatLng.latitude(), transform.getLatLng().latitude(), 1);
    ASSERT_NEAR(anchorLatLng.longitude(), transform.getLatLng().longitude(), 1);
}

TEST(Transform, Padding) {
    Transform transform;
    transform.resize({ 1000, 1000 });

    ASSERT_DOUBLE_EQ(0, transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(0, transform.getLatLng().longitude());

    transform.jumpTo(CameraOptions().withCenter(LatLng { 10, -100 }).withZoom(10.0));

    const LatLng trueCenter = transform.getLatLng();
    ASSERT_DOUBLE_EQ(10, trueCenter.latitude());
    ASSERT_DOUBLE_EQ(-100, trueCenter.longitude());
    ASSERT_DOUBLE_EQ(10, transform.getZoom());

    const LatLng manualShiftedCenter = transform.getState().screenCoordinateToLatLng({
        1000.0 / 2.0,
        1000.0 / 4.0,
    });

    EdgeInsets padding(1000.0 / 2.0, 0, 0, 0);
    const LatLng shiftedCenter = transform.getLatLng(padding);
    ASSERT_NE(trueCenter.latitude(), shiftedCenter.latitude());
    ASSERT_NEAR(trueCenter.longitude(), shiftedCenter.longitude(), 1e-9);
    ASSERT_DOUBLE_EQ(manualShiftedCenter.latitude(), shiftedCenter.latitude());
    ASSERT_DOUBLE_EQ(manualShiftedCenter.longitude(), shiftedCenter.longitude());
}

TEST(Transform, MoveBy) {
    Transform transform;
    transform.resize({ 1000, 1000 });

    transform.jumpTo(CameraOptions().withCenter(LatLng()).withZoom(10.0));

    LatLng trueCenter = transform.getLatLng();
    ASSERT_DOUBLE_EQ(0, trueCenter.latitude());
    ASSERT_DOUBLE_EQ(0, trueCenter.longitude());
    ASSERT_DOUBLE_EQ(10, transform.getZoom());

    for (uint8_t x = 0; x < 20; ++x) {
        bool odd = x % 2;
        bool forward = x % 10;

        LatLng coordinate = transform.screenCoordinateToLatLng({ odd ? 400. : 600., forward ? 400. : 600 });
        transform.moveBy({ odd ? 100. : -100., forward ? 100. : -100 });

        trueCenter = transform.getLatLng();
        ASSERT_NEAR(coordinate.latitude(), trueCenter.latitude(), 0.0001);
        ASSERT_NEAR(coordinate.longitude(), trueCenter.longitude(), 0.0001);
    }

    // We have ~1.1 precision loss for each coordinate for 20 rounds of moveBy.
    ASSERT_NEAR(0, trueCenter.latitude(), 1.1);
    ASSERT_NEAR(0, trueCenter.longitude(), 1.1);
}

TEST(Transform, Antimeridian) {
    Transform transform;
    transform.resize({ 1000, 1000 });

    transform.jumpTo(CameraOptions().withCenter(LatLng()).withZoom(1.0));

    // San Francisco
    const LatLng coordinateSanFrancisco { 37.7833, -122.4167 };
    ScreenCoordinate pixelSF = transform.latLngToScreenCoordinate(coordinateSanFrancisco);
    ASSERT_NEAR(151.79409149185352, pixelSF.x, 1e-2);
    ASSERT_NEAR(383.76774094913071, pixelSF.y, 1e-2);

    transform.jumpTo(CameraOptions().withCenter(LatLng { 0.0, -181.0 }));

    ScreenCoordinate pixelSFLongest = transform.latLngToScreenCoordinate(coordinateSanFrancisco);
    ASSERT_NEAR(-357.36306616412816, pixelSFLongest.x, 1e-2);
    ASSERT_DOUBLE_EQ(pixelSF.y, pixelSFLongest.y);
    LatLng unwrappedSF = coordinateSanFrancisco.wrapped();
    unwrappedSF.unwrapForShortestPath(transform.getLatLng());

    ScreenCoordinate pixelSFShortest = transform.latLngToScreenCoordinate(unwrappedSF);
    ASSERT_NEAR(666.63617954008976, pixelSFShortest.x, 1e-2);
    ASSERT_DOUBLE_EQ(pixelSF.y, pixelSFShortest.y);

    transform.jumpTo(CameraOptions().withCenter(LatLng { 0.0, 179.0 }));
    pixelSFShortest = transform.latLngToScreenCoordinate(coordinateSanFrancisco);
    ASSERT_DOUBLE_EQ(pixelSFLongest.x, pixelSFShortest.x);
    ASSERT_DOUBLE_EQ(pixelSFLongest.y, pixelSFShortest.y);

    // Waikiri
    const LatLng coordinateWaikiri{ -16.9310, 179.9787 };
    transform.jumpTo(CameraOptions().withCenter(coordinateWaikiri).withZoom(10.0));
    ScreenCoordinate pixelWaikiri = transform.latLngToScreenCoordinate(coordinateWaikiri);
    ASSERT_NEAR(500, pixelWaikiri.x, 1e-2);
    ASSERT_NEAR(500, pixelWaikiri.y, 1e-2);

    transform.jumpTo(CameraOptions().withCenter(LatLng { coordinateWaikiri.latitude(), 180.0213 }));
    ScreenCoordinate pixelWaikiriLongest = transform.latLngToScreenCoordinate(coordinateWaikiri);
    ASSERT_NEAR(524725.96438108233, pixelWaikiriLongest.x, 1e-2);
    ASSERT_DOUBLE_EQ(pixelWaikiri.y, pixelWaikiriLongest.y);

    LatLng unwrappedWaikiri = coordinateWaikiri.wrapped();
    unwrappedWaikiri.unwrapForShortestPath(transform.getLatLng());
    ScreenCoordinate pixelWaikiriShortest = transform.latLngToScreenCoordinate(unwrappedWaikiri);
    ASSERT_NEAR(437.95953728819512, pixelWaikiriShortest.x, 1e-2);
    ASSERT_DOUBLE_EQ(pixelWaikiri.y, pixelWaikiriShortest.y);

    LatLng coordinateFromPixel = transform.screenCoordinateToLatLng(pixelWaikiriLongest);
    ASSERT_NEAR(coordinateWaikiri.latitude(), coordinateFromPixel.latitude(), 0.000001);
    ASSERT_NEAR(coordinateWaikiri.longitude(), coordinateFromPixel.longitude(), 0.000001);

    transform.jumpTo(CameraOptions().withCenter(LatLng { coordinateWaikiri.latitude(), 180.0213 }));
    pixelWaikiriShortest = transform.latLngToScreenCoordinate(coordinateWaikiri);
    ASSERT_DOUBLE_EQ(pixelWaikiriLongest.x, pixelWaikiriShortest.x);
    ASSERT_DOUBLE_EQ(pixelWaikiriLongest.y, pixelWaikiriShortest.y);

    coordinateFromPixel = transform.screenCoordinateToLatLng(pixelWaikiriShortest);
    ASSERT_NEAR(coordinateWaikiri.latitude(), coordinateFromPixel.latitude(), 0.000001);
    ASSERT_NEAR(coordinateWaikiri.longitude(), coordinateFromPixel.longitude(), 0.000001);
}

TEST(Transform, Camera) {
    Transform transform;
    transform.resize({ 1000, 1000 });

    const LatLng latLng1 { 45, 135 };
    CameraOptions cameraOptions1 = CameraOptions().withCenter(latLng1).withZoom(20.0);
    transform.jumpTo(cameraOptions1);
    ASSERT_DOUBLE_EQ(latLng1.latitude(), transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(latLng1.longitude(), transform.getLatLng().longitude());
    ASSERT_DOUBLE_EQ(20, transform.getZoom());

    const LatLng latLng2 { -45, -135 };
    CameraOptions cameraOptions2 = CameraOptions().withCenter(latLng2).withZoom(10.0);
    transform.jumpTo(cameraOptions2);
    ASSERT_DOUBLE_EQ(latLng2.latitude(), transform.getLatLng().latitude());
    ASSERT_DOUBLE_EQ(latLng2.longitude(), transform.getLatLng().longitude());
    ASSERT_DOUBLE_EQ(10, transform.getZoom());

    AnimationOptions easeOptions(Seconds(1));
    easeOptions.transitionFrameFn = [&](double t) {
        ASSERT_TRUE(t >= 0 && t <= 1);
        ASSERT_GE(latLng1.latitude(), transform.getLatLng().latitude());
        ASSERT_LE(latLng1.longitude(), transform.getLatLng().longitude());
    };
    easeOptions.transitionFinishFn = [&]() {
        ASSERT_NEAR(latLng1.latitude(), transform.getLatLng().latitude(), 0.000001);
        ASSERT_NEAR(latLng1.longitude(), transform.getLatLng().longitude(), 0.000001);
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
        ASSERT_LE(latLng2.latitude(), transform.getLatLng().latitude());
        ASSERT_GE(latLng2.longitude(), transform.getLatLng().longitude());
    };
    flyOptions.transitionFinishFn = [&]() {
        // XXX Fix precision loss in flyTo:
        // https://github.com/mapbox/mapbox-gl-native/issues/4298
        ASSERT_NEAR(latLng2.latitude(), transform.getLatLng().latitude(), 0.001);
        ASSERT_NEAR(latLng2.longitude(), transform.getLatLng().longitude(), 0.001);
        ASSERT_NEAR(10, transform.getZoom(), 0.00001);
    };

    transform.flyTo(cameraOptions2, flyOptions);
    ASSERT_TRUE(transform.inTransition());
    transform.updateTransitions(transform.getTransitionStart() + Milliseconds(250));
    transform.updateTransitions(transform.getTransitionStart() + Milliseconds(500));
    transform.updateTransitions(transform.getTransitionStart() + Milliseconds(750));
    transform.updateTransitions(transform.getTransitionStart() + transform.getTransitionDuration());
    ASSERT_FALSE(transform.inTransition());

    // Anchor and center points are mutually exclusive.
    CameraOptions camera;
    camera.center = LatLng { 0, 0 };
    camera.anchor = ScreenCoordinate { 0, 0 }; // top-left
    camera.zoom = transform.getState().getMaxZoom();
    transform.easeTo(camera, AnimationOptions(Seconds(1)));
    transform.updateTransitions(transform.getTransitionStart() + Milliseconds(250));
    transform.updateTransitions(transform.getTransitionStart() + Milliseconds(500));
    transform.updateTransitions(transform.getTransitionStart() + Milliseconds(750));
    transform.updateTransitions(transform.getTransitionStart() + transform.getTransitionDuration());
    ASSERT_DOUBLE_EQ(transform.getLatLng().latitude(), 0);
    ASSERT_DOUBLE_EQ(transform.getLatLng().longitude(), 0);
}

TEST(Transform, DefaultTransform) {
    struct TransformObserver : public mbgl::MapObserver {
        void onCameraWillChange(MapObserver::CameraChangeMode) final {
            cameraWillChangeCallback();
        };

        void onCameraDidChange(MapObserver::CameraChangeMode) final {
            cameraDidChangeCallback();
        };

        std::function<void()> cameraWillChangeCallback;
        std::function<void()> cameraDidChangeCallback;
    };

    uint32_t cameraWillChangeCount = 0;
    uint32_t cameraDidChangeCount = 0;

    TransformObserver observer;
    observer.cameraWillChangeCallback = [&cameraWillChangeCount]() { cameraWillChangeCount++; };
    observer.cameraDidChangeCallback = [&cameraDidChangeCount]() { cameraDidChangeCount++; };

    Transform transform(observer);
    const TransformState& state = transform.getState();
    ASSERT_FALSE(state.valid());

    LatLng nullIsland, latLng = {};
    ScreenCoordinate center, point = {};
    const uint32_t min = 0;
    const uint32_t max = 65535;

    // Cannot assign invalid sizes.
    std::vector<Size> invalidSizes = { {}, { min, max }, { max, min } };
    for (const Size& size : invalidSizes) {
        try {
            transform.resize(size);
            ASSERT_TRUE(false) << "Should throw";
        } catch (...) {
            ASSERT_TRUE(size.isEmpty());
        }
    }

    Size validSize { max, max };
    ASSERT_FALSE(validSize.isEmpty());

    try {
        transform.resize(validSize);
        ASSERT_EQ(cameraWillChangeCount, 1u);
        ASSERT_EQ(cameraDidChangeCount, 1u);
    } catch (...) {
        ASSERT_TRUE(false) << "Should not throw";
    }

    ASSERT_TRUE(state.valid());

    // Double resize
    try {
        transform.resize(validSize);
        ASSERT_EQ(cameraWillChangeCount, 1u);
        ASSERT_EQ(cameraDidChangeCount, 1u);
    } catch (...) {
        ASSERT_TRUE(false) << "Should not throw";
    }

    center = { max / 2., max / 2. };
    latLng = state.screenCoordinateToLatLng(center);
    ASSERT_NEAR(latLng.latitude(), nullIsland.latitude(), 1e-5);
    ASSERT_NEAR(latLng.longitude(), nullIsland.longitude(), 1e-5);

    point = state.latLngToScreenCoordinate(nullIsland);
    ASSERT_DOUBLE_EQ(point.x, center.x);
    ASSERT_DOUBLE_EQ(point.y, center.y);
}

TEST(Transform, LatLngBounds) {
    const LatLng nullIsland {};
    const LatLng sanFrancisco { 37.7749, -122.4194 };

    Transform transform;
    transform.resize({ 1000, 1000 });

    transform.jumpTo(CameraOptions().withCenter(LatLng()).withZoom(transform.getState().getMaxZoom()));

    // Default bounds.
    ASSERT_EQ(transform.getState().getLatLngBounds(), optional<LatLngBounds> {});
    ASSERT_EQ(transform.getLatLng(), nullIsland);

    // Invalid bounds.
    try {
        transform.setLatLngBounds(LatLngBounds::empty());
        ASSERT_TRUE(false) << "Should throw";
    } catch (...) {
        ASSERT_EQ(transform.getState().getLatLngBounds(), optional<LatLngBounds> {});
    }

    transform.jumpTo(CameraOptions().withCenter(sanFrancisco));
    ASSERT_EQ(transform.getLatLng(), sanFrancisco);

    // Single location.
    transform.setLatLngBounds(LatLngBounds::singleton(sanFrancisco));
    ASSERT_EQ(transform.getLatLng(), sanFrancisco);

    transform.setLatLngBounds(LatLngBounds::hull({ -90.0, -180.0 }, { 0.0, 180.0 }));
    transform.jumpTo(CameraOptions().withCenter(sanFrancisco));
    ASSERT_EQ(transform.getLatLng().latitude(), 0.0);
    ASSERT_EQ(transform.getLatLng().longitude(), sanFrancisco.longitude());

    transform.setLatLngBounds(LatLngBounds::hull({ -90.0, 0.0 }, { 90.0, 180.0 }));
    transform.jumpTo(CameraOptions().withCenter(sanFrancisco));
    ASSERT_EQ(transform.getLatLng().latitude(), sanFrancisco.latitude());
    ASSERT_EQ(transform.getLatLng().longitude(), 0.0);

    transform.setLatLngBounds(LatLngBounds::hull({ -90.0, 0.0 }, { 0.0, 180.0 }));
    transform.jumpTo(CameraOptions().withCenter(sanFrancisco));
    ASSERT_EQ(transform.getLatLng().latitude(), 0.0);
    ASSERT_EQ(transform.getLatLng().longitude(), 0.0);
}

TEST(Transform, PitchBounds) {
    Transform transform;
    transform.resize({ 1000, 1000 });

    transform.jumpTo(CameraOptions().withCenter(LatLng()).withZoom(transform.getState().getMaxZoom()));

    ASSERT_DOUBLE_EQ(transform.getState().getPitch() * util::RAD2DEG, 0.0);
    ASSERT_DOUBLE_EQ(transform.getState().getMinPitch() * util::RAD2DEG, 0.0);
    ASSERT_DOUBLE_EQ(transform.getState().getMaxPitch() * util::RAD2DEG, 60.0);

    transform.setMinPitch(45.0 * util::DEG2RAD);
    transform.jumpTo(CameraOptions().withPitch(0));
    ASSERT_NEAR(transform.getState().getPitch() * util::RAD2DEG, 45.0, 1e-5);

    transform.setMaxPitch(55.0 * util::DEG2RAD);
    transform.jumpTo(CameraOptions().withPitch(60.0));
    ASSERT_NEAR(transform.getState().getPitch() * util::RAD2DEG, 55.0, 1e-5);
}
