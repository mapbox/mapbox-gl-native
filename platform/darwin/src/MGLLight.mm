#import "MGLLight.h"

#import "MGLTypes.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleValue_Private.h"

#import <mbgl/style/light.hpp>
#import <mbgl/style/types.hpp>

namespace mbgl {
    
    MBGL_DEFINE_ENUM(MGLLightAnchorType, {
        { MGLLightAnchorTypeMap, "map" },
        { MGLLightAnchorTypeViewport, "viewport" },
    });
    
}

@interface MGLLight()

@property (nonatomic) mbgl::style::Light *rawLight;

@end

@implementation MGLLight

- (instancetype)init
{
    if ( self == [super init] ) {
        _rawLight = new mbgl::style::Light();

    }

    return self;
}

- (instancetype)initWithRawLayer:(mbgl::style::Light *)rawLight {
    if (self = [super init]) {
        _rawLight = rawLight;
    }
    return self;
}

- (std::unique_ptr<mbgl::style::Light>)mbglLight {
//    std::unique_ptr<mbgl::style::Light> mbglLight = std::make_unique<mbgl::style::Light>();
//    mbglLight.get()->setAnchor(self.rawLight->getAnchor());
//    mbglLight.get()->setPosition(self.rawLight->getPosition());
//    mbglLight.get()->setColor(self.rawLight->getColor());
//    mbglLight.get()->setIntensity(self.rawLight->getIntensity());
    std::unique_ptr<mbgl::style::Light> mbglLight(std::move(self.rawLight));
    return mbglLight;
    
}

- (void)setLightAnchorType:(MGLStyleValue<NSValue *> *)lightAnchorType {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::LightAnchorType, NSValue *, mbgl::style::LightAnchorType, MGLLightAnchorType>().toEnumPropertyValue(lightAnchorType);
    self.rawLight->setAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lightAnchorType {
    auto propertyValue = self.rawLight->getAnchor();
    if (propertyValue.isUndefined()) {
        mbgl::style::PropertyValue<mbgl::style::LightAnchorType> defaultAnchor = self.rawLight->getDefaultAnchor();
        return MGLStyleValueTransformer<mbgl::style::LightAnchorType, NSValue *, mbgl::style::LightAnchorType, MGLLightAnchorType>().toEnumStyleValue(defaultAnchor);
    }
    return MGLStyleValueTransformer<mbgl::style::LightAnchorType, NSValue *, mbgl::style::LightAnchorType, MGLLightAnchorType>().toEnumStyleValue(propertyValue);
}

- (void)setLightAnchorTypeTransition:(MGLTransition )transition {
    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLight->setAnchorTransition(options);
}

- (MGLTransition)lightAnchorTypeTransition {
    mbgl::style::TransitionOptions transitionOptions = self.rawLight->getAnchorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setPosition:(MGLPosition)position {
    std::array<float, 3> positionArray = { static_cast<float>(position.radial), static_cast<float>(position.azimuthal), static_cast<float>(position.polar) };
    mbgl::style::Position lightPosition(positionArray);
    self.rawLight->setPosition(lightPosition);
}

- (MGLPosition)position {
    mbgl::style::PropertyValue<mbgl::style::Position> lightPosition = self.rawLight->getPosition();
    std::array<float, 3> defaultPosition;
    
    if (lightPosition.isUndefined()) {
        defaultPosition = self.rawLight->getDefaultPosition().getSpherical();
    } else {
        defaultPosition = lightPosition.asConstant().getSpherical();
    }
    
    MGLPosition position;
    position.radial = defaultPosition[0];
    position.azimuthal = defaultPosition[1];
    position.polar = defaultPosition[2];
    
    return position;

}

- (void)setPositionTransition:(MGLTransition )transition {
    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLight->setPositionTransition(options);
}

- (MGLTransition)positionTransition {
    mbgl::style::TransitionOptions transitionOptions = self.rawLight->getPositionTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));
    
    return transition;
}

- (void)setColor:(MGLStyleValue<MGLColor *> *)color {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toInterpolatablePropertyValue(color);
    self.rawLight->setColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)color{
    auto propertyValue = self.rawLight->getColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(self.rawLight->getDefaultColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setColorTransition:(MGLTransition )transition {
    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLight->setColorTransition(options);
}

- (MGLTransition)colorTransition {
    mbgl::style::TransitionOptions transitionOptions = self.rawLight->getColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));
    
    return transition;
}

- (void)setIntensity:(MGLStyleValue<NSNumber *> *)intensity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(intensity);
    self.rawLight->setIntensity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)intensity {
    auto propertyValue = self.rawLight->getIntensity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLight->getDefaultIntensity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);

}

- (void)setIntensityTransition:(MGLTransition )transition {
    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLight->setIntensityTransition(options);
}

- (MGLTransition)intensityTransition {
    mbgl::style::TransitionOptions transitionOptions = self.rawLight->getIntensityTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));
    
    return transition;
}

@end
                                     
@implementation NSValue (MGLLightAdditions)

+ (NSValue *)valueWithMGLLightAnchorType:(MGLLightAnchorType)lightAnchorType {
    return [NSValue value:&lightAnchorType withObjCType:@encode(MGLLightAnchorType)];
}

@end
