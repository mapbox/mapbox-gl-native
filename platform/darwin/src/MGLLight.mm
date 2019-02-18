// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.
// test

#import "MGLLight.h"

#import "MGLTypes.h"
#import "MGLStyleValue_Private.h"
#import "NSValue+MGLAdditions.h"
#import "MGLLoggingConfiguration_Private.h"

#import <mbgl/style/light.hpp>
#import <mbgl/style/types.hpp>

namespace mbgl {

    MBGL_DEFINE_ENUM(MGLLightAnchor, {
        { MGLLightAnchorMap, "map" },
        { MGLLightAnchorViewport, "viewport" },
    });

}

@interface MGLLight()

@end

@implementation MGLLight

- (instancetype)initWithMBGLLight:(const mbgl::style::Light *)mbglLight
{
    if (self = [super init]) {
        MGLLogInfo(@"Initializing %@.", NSStringFromClass([self class]));
        auto anchor = mbglLight->getAnchor();
        NSExpression *anchorExpression;
        if (anchor.isUndefined()) {
            mbgl::style::PropertyValue<mbgl::style::LightAnchorType> defaultAnchor = mbglLight->getDefaultAnchor();
            anchorExpression = MGLStyleValueTransformer<mbgl::style::LightAnchorType, NSValue *, mbgl::style::LightAnchorType, MGLLightAnchor>().toExpression(defaultAnchor);
        } else {
            anchorExpression = MGLStyleValueTransformer<mbgl::style::LightAnchorType, NSValue *, mbgl::style::LightAnchorType, MGLLightAnchor>().toExpression(anchor);
        }

        _anchor = anchorExpression;

        auto positionValue = mbglLight->getPosition();
        if (positionValue.isUndefined()) {
            _position = MGLStyleValueTransformer<mbgl::style::Position, NSValue *>().toExpression(mbglLight->getDefaultPosition());
        } else {
            _position = MGLStyleValueTransformer<mbgl::style::Position, NSValue *>().toExpression(positionValue);
        }
        _positionTransition = MGLTransitionFromOptions(mbglLight->getPositionTransition());
        auto colorValue = mbglLight->getColor();
        if (colorValue.isUndefined()) {
            _color = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(mbglLight->getDefaultColor());
        } else {
            _color = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(colorValue);
        }
        _colorTransition = MGLTransitionFromOptions(mbglLight->getColorTransition());
        auto intensityValue = mbglLight->getIntensity();
        if (intensityValue.isUndefined()) {
            _intensity = MGLStyleValueTransformer<float, NSNumber *>().toExpression(mbglLight->getDefaultIntensity());
        } else {
            _intensity = MGLStyleValueTransformer<float, NSNumber *>().toExpression(intensityValue);
        }
        _intensityTransition = MGLTransitionFromOptions(mbglLight->getIntensityTransition());
    }

    return self;
}

- (mbgl::style::Light)mbglLight
{
    mbgl::style::Light mbglLight;
    auto anchor = MGLStyleValueTransformer<mbgl::style::LightAnchorType, NSValue *, mbgl::style::LightAnchorType, MGLLightAnchor>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::LightAnchorType>>(self.anchor, false);
    mbglLight.setAnchor(anchor);

    auto position = MGLStyleValueTransformer<mbgl::style::Position, NSValue *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::Position>>(self.position, false);
    mbglLight.setPosition(position);

    mbglLight.setPositionTransition(MGLOptionsFromTransition(self.positionTransition));

    auto color = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(self.color, false);
    mbglLight.setColor(color);

    mbglLight.setColorTransition(MGLOptionsFromTransition(self.colorTransition));

    auto intensity = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(self.intensity, false);
    mbglLight.setIntensity(intensity);

    mbglLight.setIntensityTransition(MGLOptionsFromTransition(self.intensityTransition));


    return mbglLight;
}



- (void)setAnchor:(NSExpression *)anchor {
    MGLLogDebug(@"Setting anchor: %@", anchor);
    _anchor = anchor;
}

- (void)setPosition:(NSExpression *)position {
    MGLLogDebug(@"Setting position: %@", position);
    _position = position;
}

- (void)setPositionTransition:(MGLTransition)transition {
    MGLLogDebug(@"Setting positionTransition: %@", MGLStringFromMGLTransition(transition));
    _positionTransition = transition;
}

- (void)setColor:(NSExpression *)color {
    MGLLogDebug(@"Setting color: %@", color);
    _color = color;
}

- (void)setColorTransition:(MGLTransition)transition {
    MGLLogDebug(@"Setting colorTransition: %@", MGLStringFromMGLTransition(transition));
    _colorTransition = transition;
}

- (void)setIntensity:(NSExpression *)intensity {
    MGLLogDebug(@"Setting intensity: %@", intensity);
    _intensity = intensity;
}

- (void)setIntensityTransition:(MGLTransition)transition {
    MGLLogDebug(@"Setting intensityTransition: %@", MGLStringFromMGLTransition(transition));
    _intensityTransition = transition;
}

@end
