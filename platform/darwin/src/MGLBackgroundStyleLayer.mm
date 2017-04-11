// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLBackgroundStyleLayer.h"

#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/layers/background_layer.hpp>

@interface MGLBackgroundStyleLayer ()

@property (nonatomic, readonly) mbgl::style::BackgroundLayer *rawLayer;

@end

@implementation MGLBackgroundStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier
{
    auto layer = std::make_unique<mbgl::style::BackgroundLayer>(identifier.UTF8String);
    return self = [super initWithPendingLayer:std::move(layer)];
}

- (mbgl::style::BackgroundLayer *)rawLayer
{
    return (mbgl::style::BackgroundLayer *)super.rawLayer;
}

#pragma mark - Accessing the Paint Attributes

- (void)setBackgroundColor:(MGLStyleValue<MGLColor *> *)backgroundColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toInterpolatablePropertyValue(backgroundColor);
    self.rawLayer->setBackgroundColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)backgroundColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getBackgroundColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(self.rawLayer->getDefaultBackgroundColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setBackgroundColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setBackgroundColorTransition(options);
}

- (MGLTransition)backgroundColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getBackgroundColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setBackgroundOpacity:(MGLStyleValue<NSNumber *> *)backgroundOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(backgroundOpacity);
    self.rawLayer->setBackgroundOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)backgroundOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getBackgroundOpacity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultBackgroundOpacity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setBackgroundOpacityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setBackgroundOpacityTransition(options);
}

- (MGLTransition)backgroundOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getBackgroundOpacityTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setBackgroundPattern:(MGLStyleValue<NSString *> *)backgroundPattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(backgroundPattern);
    self.rawLayer->setBackgroundPattern(mbglValue);
}

- (MGLStyleValue<NSString *> *)backgroundPattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getBackgroundPattern();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(self.rawLayer->getDefaultBackgroundPattern());
    }
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setBackgroundPatternTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setBackgroundPatternTransition(options);
}

- (MGLTransition)backgroundPatternTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getBackgroundPatternTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

@end
