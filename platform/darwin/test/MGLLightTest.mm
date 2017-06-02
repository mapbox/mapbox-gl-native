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
        
        NSAssert([mglLight.anchor isKindOfClass:[MGLConstantStyleValue class]], @"mglLight.anchor isn’t a MGLConstantStyleValue.");
        NSValue *anchorValue = ((MGLConstantStyleValue *)mglLight.anchor).rawValue;
        XCTAssertEqual(anchorValue.MGLLightAnchorValue, MGLLightAnchorViewport);
        XCTAssertEqual(mglLight.anchorTransition.delay, defaultTransition.delay);
        XCTAssertEqual(mglLight.anchorTransition.duration, defaultTransition.duration);
        
        auto lightFromMGLlight = [mglLight mbglLight];
        
        XCTAssertEqual(light.getDefaultAnchor(), lightFromMGLlight.getAnchor().asConstant());
        auto anchorTransition = lightFromMGLlight.getAnchorTransition();
        XCTAssert(anchorTransition.delay && MGLTimeIntervalFromDuration(*anchorTransition.delay) == defaultTransition.delay);
        XCTAssert(anchorTransition.duration && MGLTimeIntervalFromDuration(*anchorTransition.duration) == defaultTransition.duration);
        
        MGLStyleValue<NSValue *> *anchorStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLLightAnchor:MGLLightAnchorMap]];
        mglLight.anchor = anchorStyleValue;
        mglLight.anchorTransition = transition;
        NSAssert([mglLight.anchor isKindOfClass:[MGLConstantStyleValue class]], @"mglLight.anchor isn’t a MGLConstantStyleValue.");
        anchorValue = ((MGLConstantStyleValue *)mglLight.anchor).rawValue;
        
        XCTAssertEqual(anchorValue.MGLLightAnchorValue, MGLLightAnchorMap);
        XCTAssertEqual(mglLight.anchorTransition.delay, transition.delay);
        XCTAssertEqual(mglLight.anchorTransition.duration, transition.duration);
        
        mbgl::style::PropertyValue<mbgl::style::LightAnchorType> anchorProperty = {  mbgl::style::LightAnchorType::Map  };
        light.setAnchor(anchorProperty);
        light.setAnchorTransition(transitionOptions);
        
        lightFromMGLlight = [mglLight mbglLight];
        
        XCTAssertEqual(light.getAnchor().asConstant(), lightFromMGLlight.getAnchor().asConstant());
        anchorTransition = lightFromMGLlight.getAnchorTransition();
        XCTAssert(anchorTransition.delay && MGLTimeIntervalFromDuration(*anchorTransition.delay) == transition.delay);
        XCTAssert(anchorTransition.duration && MGLTimeIntervalFromDuration(*anchorTransition.duration) == transition.duration);
        
    }
    
    // position
    {
        mbgl::style::Light light;
        MGLLight *mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        NSAssert([mglLight.position isKindOfClass:[MGLConstantStyleValue class]], @"mglLight.position isn’t a MGLConstantStyleValue.");
        NSValue *positionValue = ((MGLConstantStyleValue *)mglLight.position).rawValue;
        auto positionArray = light.getDefaultPosition().getSpherical();
        MGLSphericalPosition defaultPosition = MGLSphericalPositionMake(positionArray[0], positionArray[1], positionArray[2]);
        
        XCTAssert(defaultPosition.radial == positionValue.MGLSphericalPositionValue.radial);
        XCTAssert(defaultPosition.azimuthal == positionValue.MGLSphericalPositionValue.azimuthal);
        XCTAssert(defaultPosition.polar == positionValue.MGLSphericalPositionValue.polar);
        XCTAssertEqual(mglLight.positionTransiton.delay, defaultTransition.delay);
        XCTAssertEqual(mglLight.positionTransiton.duration, defaultTransition.duration);
        
        auto lightFromMGLlight = [mglLight mbglLight];
        
        XCTAssertEqual(positionArray, lightFromMGLlight.getPosition().asConstant().getSpherical());
        auto positionTransition = lightFromMGLlight.getPositionTransition();
        XCTAssert(positionTransition.delay && MGLTimeIntervalFromDuration(*positionTransition.delay) == defaultTransition.delay);
        XCTAssert(positionTransition.duration && MGLTimeIntervalFromDuration(*positionTransition.duration) == defaultTransition.duration);
        
        defaultPosition = MGLSphericalPositionMake(6, 180, 90);
        MGLStyleValue<NSValue *> *positionStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLSphericalPosition:defaultPosition]];
        mglLight.position = positionStyleValue;
        mglLight.positionTransiton = transition;
        
        NSAssert([mglLight.position isKindOfClass:[MGLConstantStyleValue class]], @"mglLight.position isn’t a MGLConstantStyleValue.");
        positionValue = ((MGLConstantStyleValue *)mglLight.position).rawValue;
        
        XCTAssert(defaultPosition.radial == positionValue.MGLSphericalPositionValue.radial);
        XCTAssert(defaultPosition.azimuthal == positionValue.MGLSphericalPositionValue.azimuthal);
        XCTAssert(defaultPosition.polar == positionValue.MGLSphericalPositionValue.polar);
        XCTAssertEqual(mglLight.positionTransiton.delay, transition.delay);
        XCTAssertEqual(mglLight.positionTransiton.duration, transition.duration);
        
        lightFromMGLlight = [mglLight mbglLight];
        
        positionArray = { { 6, 180, 90 } };
        mbgl::style::Position position = { positionArray };
        mbgl::style::PropertyValue<mbgl::style::Position> positionProperty = { position };
        light.setPosition(positionProperty);
        light.setPositionTransition(transitionOptions);

        XCTAssertEqual(positionArray, lightFromMGLlight.getPosition().asConstant().getSpherical());
        positionTransition = lightFromMGLlight.getPositionTransition();
        XCTAssert(positionTransition.delay && MGLTimeIntervalFromDuration(*positionTransition.delay) == transition.delay);
        XCTAssert(positionTransition.duration && MGLTimeIntervalFromDuration(*positionTransition.duration) == transition.duration);

    }
    
    // color
    {
        mbgl::style::Light light;
        MGLLight *mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        NSAssert([mglLight.color isKindOfClass:[MGLConstantStyleValue class]], @"mglLight.color isn’t a MGLConstantStyleValue.");
        MGLColor *colorValue = ((MGLConstantStyleValue *)mglLight.color).rawValue;
        auto color = light.getDefaultColor();
        const CGFloat *colorComponents = CGColorGetComponents(colorValue.CGColor);
        
        XCTAssert(color.r == colorComponents[0] && color.g == colorComponents[1] && color.b == colorComponents[2] &&
                  color.a == colorComponents[3]);
        XCTAssertEqual(mglLight.colorTransiton.delay, defaultTransition.delay);
        XCTAssertEqual(mglLight.colorTransiton.duration, defaultTransition.duration);
        
        auto lightFromMGLlight = [mglLight mbglLight];
        
        XCTAssertEqual(color, lightFromMGLlight.getColor().asConstant());
        auto colorTransition = lightFromMGLlight.getColorTransition();
        XCTAssert(colorTransition.delay && MGLTimeIntervalFromDuration(*colorTransition.delay) == defaultTransition.delay);
        XCTAssert(colorTransition.duration && MGLTimeIntervalFromDuration(*colorTransition.duration) == defaultTransition.duration);
        
        MGLStyleValue<MGLColor *> *colorStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor blackColor]];
        mglLight.color = colorStyleValue;
        mglLight.colorTransiton = transition;
        
        NSAssert([mglLight.color isKindOfClass:[MGLConstantStyleValue class]], @"mglLight.color isn’t a MGLConstantStyleValue.");
        colorValue = ((MGLConstantStyleValue *)mglLight.color).rawValue;
        
        XCTAssertEqual([MGLColor blackColor], colorValue);
        XCTAssertEqual(mglLight.colorTransiton.delay, transition.delay);
        XCTAssertEqual(mglLight.colorTransiton.duration, transition.duration);
        
        mbgl::style::PropertyValue<mbgl::Color> colorProperty = { { 0, 0, 0, 1 } };
        light.setColor(colorProperty);
        light.setColorTransition(transitionOptions);
        
        lightFromMGLlight = [mglLight mbglLight];
        
        colorComponents = CGColorGetComponents(colorValue.CGColor);
        color = lightFromMGLlight.getColor().asConstant();
        XCTAssertEqual(light.getColor().asConstant(),lightFromMGLlight.getColor().asConstant());
        colorTransition = lightFromMGLlight.getColorTransition();
        XCTAssert(colorTransition.delay && MGLTimeIntervalFromDuration(*colorTransition.delay) == transition.delay);
        XCTAssert(colorTransition.duration && MGLTimeIntervalFromDuration(*colorTransition.duration) == transition.duration);
    }
    
    // intensity
    {
        mbgl::style::Light light;
        MGLLight *mglLight = [[MGLLight alloc] initWithMBGLLight:&light];
        NSAssert([mglLight.intensity isKindOfClass:[MGLConstantStyleValue class]], @"mglLight.intensity isn’t a MGLConstantStyleValue.");
        NSNumber *intensityNumber = ((MGLConstantStyleValue *)mglLight.intensity).rawValue;
        auto intensity = light.getDefaultIntensity();
        
        XCTAssert(intensityNumber.floatValue == intensity);
        XCTAssertEqual(mglLight.intensityTransition.delay, defaultTransition.delay);
        XCTAssertEqual(mglLight.intensityTransition.duration, defaultTransition.duration);
        
        auto lightFromMGLlight = [mglLight mbglLight];
        
        XCTAssertEqual(intensity, lightFromMGLlight.getIntensity().asConstant());
        auto intensityTransition = lightFromMGLlight.getIntensityTransition();
        XCTAssert(intensityTransition.delay && MGLTimeIntervalFromDuration(*intensityTransition.delay) == defaultTransition.delay);
        XCTAssert(intensityTransition.duration && MGLTimeIntervalFromDuration(*intensityTransition.duration) == defaultTransition.duration);
        
        NSNumber *intensityValue = @0.4;
        MGLStyleValue<NSNumber *> *intensityStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:intensityValue];
        mglLight.intensity = intensityStyleValue;
        mglLight.intensityTransition = transition;
        
        NSAssert([mglLight.intensity isKindOfClass:[MGLConstantStyleValue class]], @"mglLight.intensity isn’t a MGLConstantStyleValue.");
        intensityNumber = ((MGLConstantStyleValue *)mglLight.intensity).rawValue;
        XCTAssert(intensityNumber.floatValue == intensityValue.floatValue);
        XCTAssertEqual(mglLight.intensityTransition.delay, transition.delay);
        XCTAssertEqual(mglLight.intensityTransition.duration, transition.duration);

        mbgl::style::PropertyValue<float> intensityProperty = { 0.4 };
        light.setIntensity(intensityProperty);
        light.setIntensityTransition(transitionOptions);

        lightFromMGLlight = [mglLight mbglLight];
        
        XCTAssertEqual(light.getIntensity().asConstant(), lightFromMGLlight.getIntensity().asConstant());
        intensityTransition = lightFromMGLlight.getIntensityTransition();
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
