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
        auto lightFromMGLlight = [mglLight mbglLight];

        XCTAssertEqual(light.getDefaultAnchor(), lightFromMGLlight.getAnchor());
        XCTAssert([mglLight.anchor isKindOfClass:[MGLConstantStyleValue class]], @"mglLight.anchor isn’t a MGLConstantStyleValue.");
        NSValue *anchorValue = ((MGLConstantStyleValue *)mglLight.anchor).rawValue;
        XCTAssertEqual(anchorValue.MGLLightAnchorValue, MGLLightAnchorViewport);

        mbgl::style::PropertyValue<mbgl::style::LightAnchorType> propertyValue = { mbgl::style::LightAnchorType::Viewport };
 
        light.setAnchor(propertyValue);
 
        mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        lightFromMGLlight = [mglLight mbglLight];

        XCTAssertEqual(light.getAnchor(), lightFromMGLlight.getAnchor());
    }

    // position
    {
        mbgl::style::Light light;
        MGLLight *mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        auto lightFromMGLlight = [mglLight mbglLight];

        XCTAssertEqual(light.getDefaultPosition(), lightFromMGLlight.getPosition());
        auto positionTransition = lightFromMGLlight.getPositionTransition();
        XCTAssert(positionTransition.delay && MGLTimeIntervalFromDuration(*positionTransition.delay) == defaultTransition.delay);
        XCTAssert(positionTransition.duration && MGLTimeIntervalFromDuration(*positionTransition.duration) == defaultTransition.duration);

        const std::array<float, 3> positionArray = { { 6, 180, 90 } };
        mbgl::style::Position position = { positionArray };
        mbgl::style::PropertyValue<mbgl::style::Position> propertyValue = { position };
 
        light.setPosition(propertyValue);
        light.setPositionTransition(transitionOptions);

 
        mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        lightFromMGLlight = [mglLight mbglLight];

        XCTAssertEqual(light.getPosition(), lightFromMGLlight.getPosition());
        positionTransition = lightFromMGLlight.getPositionTransition();
        XCTAssert(positionTransition.delay && MGLTimeIntervalFromDuration(*positionTransition.delay) == transition.delay);
        XCTAssert(positionTransition.duration && MGLTimeIntervalFromDuration(*positionTransition.duration) == transition.duration);

    }

    // color
    {
        mbgl::style::Light light;
        MGLLight *mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        auto lightFromMGLlight = [mglLight mbglLight];

        XCTAssertEqual(light.getDefaultColor(), lightFromMGLlight.getColor());
        auto colorTransition = lightFromMGLlight.getColorTransition();
        XCTAssert(colorTransition.delay && MGLTimeIntervalFromDuration(*colorTransition.delay) == defaultTransition.delay);
        XCTAssert(colorTransition.duration && MGLTimeIntervalFromDuration(*colorTransition.duration) == defaultTransition.duration);

        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
 
        light.setColor(propertyValue);
        light.setColorTransition(transitionOptions);

 
        mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        lightFromMGLlight = [mglLight mbglLight];

        XCTAssertEqual(light.getColor(), lightFromMGLlight.getColor());
        colorTransition = lightFromMGLlight.getColorTransition();
        XCTAssert(colorTransition.delay && MGLTimeIntervalFromDuration(*colorTransition.delay) == transition.delay);
        XCTAssert(colorTransition.duration && MGLTimeIntervalFromDuration(*colorTransition.duration) == transition.duration);

    }

    // intensity
    {
        mbgl::style::Light light;
        MGLLight *mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        auto lightFromMGLlight = [mglLight mbglLight];

        XCTAssertEqual(light.getDefaultIntensity(), lightFromMGLlight.getIntensity());
        auto intensityTransition = lightFromMGLlight.getIntensityTransition();
        XCTAssert(intensityTransition.delay && MGLTimeIntervalFromDuration(*intensityTransition.delay) == defaultTransition.delay);
        XCTAssert(intensityTransition.duration && MGLTimeIntervalFromDuration(*intensityTransition.duration) == defaultTransition.duration);

        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
 
        light.setIntensity(propertyValue);
        light.setIntensityTransition(transitionOptions);

 
        mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        lightFromMGLlight = [mglLight mbglLight];

        XCTAssertEqual(light.getIntensity(), lightFromMGLlight.getIntensity());
        intensityTransition = lightFromMGLlight.getIntensityTransition();
        XCTAssert(intensityTransition.delay && MGLTimeIntervalFromDuration(*intensityTransition.delay) == transition.delay);
        XCTAssert(intensityTransition.duration && MGLTimeIntervalFromDuration(*intensityTransition.duration) == transition.duration);

    }

}

- (void)testValueAdditions {
    MGLSphericalPosition position = MGLSphericalPositionMake(1.15, 210, 30);
    
    XCTAssertEqual(@(position).MGLSphericalPositionValue.radial, position.radial);
    XCTAssertEqual(@(position).MGLSphericalPositionValue.azimuthal, position.azimuthal);
    XCTAssertEqual(@(position).MGLSphericalPositionValue.polar, position.polar);
    XCTAssertEqual([NSValue valueWithMGLLightAnchor:MGLLightAnchorMap].MGLLightAnchorValue, MGLLightAnchorMap);
    XCTAssertEqual([NSValue valueWithMGLLightAnchor:MGLLightAnchorViewport].MGLLightAnchorValue, MGLLightAnchorViewport);
}

@end
