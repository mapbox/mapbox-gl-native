// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLPrivateAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLBackgroundStyleLayer.h"
#import "MGLLoggingConfiguration_Private.h"
#import "MGLBackgroundStyleLayer_Private.h"

#include <mbgl/style/transition_options.hpp>


@interface MGLBackgroundStyleLayer ()

@property (nonatomic, readonly) mbgl::style::BackgroundLayer *rawLayer;

@end

@implementation MGLBackgroundStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier
{
    MGLLogDebug(@"Initializing %@ with identifier: %@", NSStringFromClass([self class]), identifier);
    auto layer = std::make_unique<mbgl::style::BackgroundLayer>(identifier.UTF8String);
    return self = [super initWithPendingLayer:std::move(layer)];
}

- (mbgl::style::BackgroundLayer *)rawLayer
{
    return (mbgl::style::BackgroundLayer *)super.rawLayer;
}

#pragma mark - Accessing the Paint Attributes

- (void)setBackgroundColor:(NSExpression *)backgroundColor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting backgroundColor: %@", backgroundColor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(backgroundColor, false);
    self.rawLayer->setBackgroundColor(mbglValue);
}

- (NSExpression *)backgroundColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getBackgroundColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultBackgroundColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
}

- (void)setBackgroundColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting backgroundColorTransition: %@", MGLStringFromMGLTransition(transition));

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

- (void)setBackgroundOpacity:(NSExpression *)backgroundOpacity {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting backgroundOpacity: %@", backgroundOpacity);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(backgroundOpacity, false);
    self.rawLayer->setBackgroundOpacity(mbglValue);
}

- (NSExpression *)backgroundOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getBackgroundOpacity();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultBackgroundOpacity();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setBackgroundOpacityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting backgroundOpacityTransition: %@", MGLStringFromMGLTransition(transition));

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

- (void)setBackgroundPattern:(NSExpression *)backgroundPattern {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting backgroundPattern: %@", backgroundPattern);

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue<mbgl::style::PropertyValue<std::string>>(backgroundPattern, false);
    self.rawLayer->setBackgroundPattern(mbglValue);
}

- (NSExpression *)backgroundPattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getBackgroundPattern();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultBackgroundPattern();
    }
    return MGLStyleValueTransformer<std::string, NSString *>().toExpression(propertyValue);
}

- (void)setBackgroundPatternTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting backgroundPatternTransition: %@", MGLStringFromMGLTransition(transition));

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

namespace mbgl {

MGLStyleLayer* BackgroundStyleLayerPeerFactory::createPeer(style::Layer* rawLayer) {
    return [[MGLBackgroundStyleLayer alloc] initWithRawLayer:rawLayer];
}

}  // namespace mbgl
