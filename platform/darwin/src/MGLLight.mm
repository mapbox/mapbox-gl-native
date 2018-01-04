// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.
// test

#import "MGLLight.h"

#import "MGLTypes.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleValue_Private.h"
#import "NSValue+MGLAdditions.h"

#import <mbgl/style/light.hpp>
#import <mbgl/style/types.hpp>

namespace mbgl {

    MBGL_DEFINE_ENUM(MGLLightAnchor, {
        { MGLLightAnchorMap, "map" },
        { MGLLightAnchorViewport, "viewport" },
    });
    
}

NS_INLINE MGLTransition MGLTransitionFromOptions(const mbgl::style::TransitionOptions& options) {
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(options.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(options.delay.value_or(mbgl::Duration::zero()));
    
    return transition;
}

NS_INLINE mbgl::style::TransitionOptions MGLOptionsFromTransition(MGLTransition transition) {
    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    return options;
}

@interface MGLLight()

@end

@implementation MGLLight

- (instancetype)initWithMBGLLight:(const mbgl::style::Light *)mbglLight
{
    if (self = [super init]) {
        auto anchor = mbglLight->getAnchor();
        MGLStyleValue<NSValue *> *anchorStyleValue;
        if (anchor.isUndefined()) {
            mbgl::style::PropertyValue<mbgl::style::LightAnchorType> defaultAnchor = mbglLight->getDefaultAnchor();
            anchorStyleValue = MGLStyleValueTransformer<mbgl::style::LightAnchorType, NSValue *, mbgl::style::LightAnchorType, MGLLightAnchor>().toEnumStyleValue(defaultAnchor);
        } else {
            anchorStyleValue = MGLStyleValueTransformer<mbgl::style::LightAnchorType, NSValue *, mbgl::style::LightAnchorType, MGLLightAnchor>().toEnumStyleValue(anchor);
        }

        _anchor = anchorStyleValue;

        auto positionValue = mbglLight->getPosition();
        if (positionValue.isUndefined()) {
            _position = MGLStyleValueTransformer<mbgl::style::Position, NSValue *>().toStyleValue(mbglLight->getDefaultPosition());
        } else {
            _position = MGLStyleValueTransformer<mbgl::style::Position, NSValue *>().toStyleValue(positionValue);
        }
        
        _positionTransition = MGLTransitionFromOptions(mbglLight->getPositionTransition());
 
        auto colorValue = mbglLight->getColor();
        if (colorValue.isUndefined()) {
            _color = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(mbglLight->getDefaultColor());
        } else {
            _color = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(colorValue);
        }
        
        _colorTransition = MGLTransitionFromOptions(mbglLight->getColorTransition());
 
        auto intensityValue = mbglLight->getIntensity();
        if (intensityValue.isUndefined()) {
            _intensity = MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(mbglLight->getDefaultIntensity());
        } else {
            _intensity = MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(intensityValue);
        }
        
        _intensityTransition = MGLTransitionFromOptions(mbglLight->getIntensityTransition());
 
    }

    return self;
}

- (mbgl::style::Light)mbglLight
{
    mbgl::style::Light mbglLight;
    auto anchor = MGLStyleValueTransformer<mbgl::style::LightAnchorType, NSValue *, mbgl::style::LightAnchorType, MGLLightAnchor>().toEnumPropertyValue(self.anchor);
    mbglLight.setAnchor(anchor);

    auto position = MGLStyleValueTransformer<mbgl::style::Position, NSValue *>().toInterpolatablePropertyValue(self.position);
    mbglLight.setPosition(position);
 
    mbglLight.setPositionTransition(MGLOptionsFromTransition(self.positionTransition));

    auto color = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toInterpolatablePropertyValue(self.color);
    mbglLight.setColor(color);
 
    mbglLight.setColorTransition(MGLOptionsFromTransition(self.colorTransition));

    auto intensity = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(self.intensity);
    mbglLight.setIntensity(intensity);
 
    mbglLight.setIntensityTransition(MGLOptionsFromTransition(self.intensityTransition));

    
    return mbglLight;
}

@end
