#import "MGLLight.h"

#import "MGLTypes.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleValue_Private.h"
#import "NSValue+MGLAdditions.h"

#import <mbgl/style/light.hpp>
#import <mbgl/style/types.hpp>

namespace mbgl {
    
    MBGL_DEFINE_ENUM(MGLLightAnchor, {
        { MGLLightAnchorTypeMap, "map" },
        { MGLLightAnchorTypeViewport, "viewport" },
    });
    
}

@interface MGLLight()

@end

@implementation MGLLight

+ (instancetype)defaultLight
{
    return nil;
}

- (instancetype)initWithMBGLLight:(const mbgl::style::Light &)mbglLight
{
    if (self = [super init]) {
        auto propertyValue = mbglLight.getAnchor();
        MGLStyleValue<NSValue *> *lightAnchorType;
        if (propertyValue.isUndefined()) {
            mbgl::style::PropertyValue<mbgl::style::LightAnchorType> defaultAnchor = mbglLight.getDefaultAnchor();
            lightAnchorType = MGLStyleValueTransformer<mbgl::style::LightAnchorType, NSValue *, mbgl::style::LightAnchorType, MGLLightAnchor>().toEnumStyleValue(defaultAnchor);
        } else {
            lightAnchorType = MGLStyleValueTransformer<mbgl::style::LightAnchorType, NSValue *, mbgl::style::LightAnchorType, MGLLightAnchor>().toEnumStyleValue(propertyValue);
        }
        
        if ([lightAnchorType isKindOfClass:[MGLConstantStyleValue class]]) {
            NSValue *anchorValue = ((MGLConstantStyleValue *)lightAnchorType).rawValue;
            _lightAnchor = [anchorValue MGLLightAnchorValue];
        }
        
        _lightAnchorTransition = [self transitionFromOptions:mbglLight.getAnchorTransition()];
        
        auto positionValue = mbglLight.getPosition();
        if (positionValue.isUndefined()) {
            _position = MGLStyleValueTransformer<mbgl::style::Position, NSValue *>().toStyleValue(mbglLight.getDefaultPosition());
        } else {
            _position = MGLStyleValueTransformer<mbgl::style::Position, NSValue *>().toStyleValue(positionValue);
        }
        
        _positionTransiton = [self transitionFromOptions:mbglLight.getPositionTransition()];
        
        auto colorValue = mbglLight.getColor();
        if (colorValue.isUndefined()) {
            _color = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(mbglLight.getDefaultColor());
        } else {
            _color = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(colorValue);
        }
        
        _colorTransiton = [self transitionFromOptions:mbglLight.getColorTransition()];
        
        auto intensityValue = mbglLight.getIntensity();
        if (intensityValue.isUndefined()) {
            _intensity = MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(mbglLight.getDefaultIntensity());
        } else {
            _intensity = MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(intensityValue);
        }
        
        _intensityTransition = [self transitionFromOptions:mbglLight.getIntensityTransition()];
    }

    return self;

}

- (MGLTransition)transitionFromOptions:(const mbgl::style::TransitionOptions &)options {
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(options.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(options.delay.value_or(mbgl::Duration::zero()));
    
    return transition;
}

- (mbgl::style::Light)toMBGLLight
{
    mbgl::style::Light mbglLight;
    
    MGLStyleValue<NSValue *> *lightAnchorType = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLLightAnchor:self.lightAnchor]];
    auto anchor = MGLStyleValueTransformer<mbgl::style::LightAnchorType, NSValue *, mbgl::style::LightAnchorType, MGLLightAnchor>().toEnumPropertyValue(lightAnchorType);
    mbglLight.setAnchor(anchor);
    
    
    mbglLight.setAnchorTransition([self optionsFromMGLTransition:self.lightAnchorTransition]);
    
    auto position = MGLStyleValueTransformer<mbgl::style::Position, NSValue *>().toPropertyValue(self.position);
    mbglLight.setPosition(position);
    
    mbglLight.setPositionTransition([self optionsFromMGLTransition:self.positionTransiton]);
    
    auto color = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toInterpolatablePropertyValue(self.color);
    mbglLight.setColor(color);
    
    mbglLight.setColorTransition([self optionsFromMGLTransition:self.colorTransiton]);
    
    auto intensity = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(self.intensity);
    mbglLight.setIntensity(intensity);
    
    mbglLight.setIntensityTransition([self optionsFromMGLTransition:self.intensityTransition]);
    
    return mbglLight;
}

- (mbgl::style::TransitionOptions)optionsFromMGLTransition:(MGLTransition)transition
{
    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    return options;
    
}

@end
