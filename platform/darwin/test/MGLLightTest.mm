// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.
#import <XCTest/XCTest.h>
#import <Mapbox/Mapbox.h>

#import "MGLLight_Private.h"

#import "../../darwin/src/NSDate+MGLAdditions.h"

#import <mbgl/style/light.hpp>
#import <mbgl/style/types.hpp>
#include <mbgl/style/transition_options.hpp>

@interface MGLLightTest : XCTestCase

@end

@implementation MGLLightTest

- (void)testProperties {

    MGLTransition defaultTransition = MGLTransitionMake(0, 0);
    MGLTransition transition = MGLTransitionMake(6, 3);
    mbgl::style::TransitionOptions transitionOptions { { MGLDurationFromTimeInterval(6) }, { MGLDurationFromTimeInterval(3) } };

    // anchor
    {
        mbgl::style::Light light;
        MGLLight *mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        auto lightFromMGLLight = mglLight.mbglLight;

        XCTAssertEqual(light.getDefaultAnchor(), lightFromMGLLight.getAnchor());
        XCTAssertEqual(mglLight.anchor.expressionType, NSConstantValueExpressionType, @"mglLight.anchor isn’t a constant value expression.");
        XCTAssertEqualObjects(mglLight.anchor.constantValue, @"viewport");

        mbgl::style::PropertyValue<mbgl::style::LightAnchorType> propertyValue = { mbgl::style::LightAnchorType::Viewport };
        light.setAnchor(propertyValue);
        mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        lightFromMGLLight = mglLight.mbglLight;

        XCTAssertEqual(light.getAnchor(), lightFromMGLLight.getAnchor());
    }

    // position
    {
        mbgl::style::Light light;
        MGLLight *mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        auto lightFromMGLLight = mglLight.mbglLight;

        XCTAssertEqual(light.getDefaultPosition(), lightFromMGLLight.getPosition());
        auto positionTransition = lightFromMGLLight.getPositionTransition();
        XCTAssert(positionTransition.delay && MGLTimeIntervalFromDuration(*positionTransition.delay) == defaultTransition.delay);
        XCTAssert(positionTransition.duration && MGLTimeIntervalFromDuration(*positionTransition.duration) == defaultTransition.duration);

        std::array<float, 3> positionArray = { { 6, 180, 90 } };
        mbgl::style::Position position = { positionArray };
        mbgl::style::PropertyValue<mbgl::style::Position> propertyValue = { position };
        light.setPosition(propertyValue);
        light.setPositionTransition(transitionOptions);

        mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        lightFromMGLLight = mglLight.mbglLight;

        XCTAssertEqual(light.getPosition(), lightFromMGLLight.getPosition());
        positionTransition = lightFromMGLLight.getPositionTransition();
        XCTAssert(positionTransition.delay && MGLTimeIntervalFromDuration(*positionTransition.delay) == transition.delay);
        XCTAssert(positionTransition.duration && MGLTimeIntervalFromDuration(*positionTransition.duration) == transition.duration);

    }

    // color
    {
        mbgl::style::Light light;
        MGLLight *mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        auto lightFromMGLLight = mglLight.mbglLight;

        XCTAssertEqual(light.getDefaultColor(), lightFromMGLLight.getColor());
        auto colorTransition = lightFromMGLLight.getColorTransition();
        XCTAssert(colorTransition.delay && MGLTimeIntervalFromDuration(*colorTransition.delay) == defaultTransition.delay);
        XCTAssert(colorTransition.duration && MGLTimeIntervalFromDuration(*colorTransition.duration) == defaultTransition.duration);

        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        light.setColor(propertyValue);
        light.setColorTransition(transitionOptions);

        mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        lightFromMGLLight = mglLight.mbglLight;

        XCTAssertEqual(light.getColor(), lightFromMGLLight.getColor());
        colorTransition = lightFromMGLLight.getColorTransition();
        XCTAssert(colorTransition.delay && MGLTimeIntervalFromDuration(*colorTransition.delay) == transition.delay);
        XCTAssert(colorTransition.duration && MGLTimeIntervalFromDuration(*colorTransition.duration) == transition.duration);

    }

    // intensity
    {
        mbgl::style::Light light;
        MGLLight *mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        auto lightFromMGLLight = mglLight.mbglLight;

        XCTAssertEqual(light.getDefaultIntensity(), lightFromMGLLight.getIntensity());
        auto intensityTransition = lightFromMGLLight.getIntensityTransition();
        XCTAssert(intensityTransition.delay && MGLTimeIntervalFromDuration(*intensityTransition.delay) == defaultTransition.delay);
        XCTAssert(intensityTransition.duration && MGLTimeIntervalFromDuration(*intensityTransition.duration) == defaultTransition.duration);

        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        light.setIntensity(propertyValue);
        light.setIntensityTransition(transitionOptions);

        mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        lightFromMGLLight = mglLight.mbglLight;

        XCTAssertEqual(light.getIntensity(), lightFromMGLLight.getIntensity());
        intensityTransition = lightFromMGLLight.getIntensityTransition();
        XCTAssert(intensityTransition.delay && MGLTimeIntervalFromDuration(*intensityTransition.delay) == transition.delay);
        XCTAssert(intensityTransition.duration && MGLTimeIntervalFromDuration(*intensityTransition.duration) == transition.duration);

    }

}

- (void)testValueAdditions {
    MGLSphericalPosition position = MGLSphericalPositionMake(1.15, 210, 30);

    XCTAssertEqual([NSValue valueWithMGLSphericalPosition:position].MGLSphericalPositionValue.radial, position.radial);
    XCTAssertEqual([NSValue valueWithMGLSphericalPosition:position].MGLSphericalPositionValue.azimuthal, position.azimuthal);
    XCTAssertEqual([NSValue valueWithMGLSphericalPosition:position].MGLSphericalPositionValue.polar, position.polar);
    XCTAssertEqual([NSValue valueWithMGLLightAnchor:MGLLightAnchorMap].MGLLightAnchorValue, MGLLightAnchorMap);
    XCTAssertEqual([NSValue valueWithMGLLightAnchor:MGLLightAnchorViewport].MGLLightAnchorValue, MGLLightAnchorViewport);
}

@end
