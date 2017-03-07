#import "MGLStyleLayer_Private.h"
#import "MGLMapView_Private.h"

#include <mbgl/style/layer.hpp>

@interface MGLStyleLayer ()

@property (nonatomic) mbgl::style::Layer *rawLayer;

@end

@implementation MGLStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier
{
    if (self = [super init]) {
        _identifier = identifier;
    }
    return self;
}

- (void)setVisible:(BOOL)visible
{
    MGLAssertStyleLayerIsValid();

    mbgl::style::VisibilityType v = visible
    ? mbgl::style::VisibilityType::Visible
    : mbgl::style::VisibilityType::None;
    self.rawLayer->setVisibility(v);
}

- (BOOL)isVisible
{
    MGLAssertStyleLayerIsValid();

    mbgl::style::VisibilityType v = self.rawLayer->getVisibility();
    return (v == mbgl::style::VisibilityType::Visible);
}

- (void)setMaximumZoomLevel:(float)maximumZoomLevel
{
    MGLAssertStyleLayerIsValid();

    self.rawLayer->setMaxZoom(maximumZoomLevel);
}

- (float)maximumZoomLevel
{
    MGLAssertStyleLayerIsValid();

    return self.rawLayer->getMaxZoom();
}

- (void)setMinimumZoomLevel:(float)minimumZoomLevel
{
    MGLAssertStyleLayerIsValid();

    self.rawLayer->setMinZoom(minimumZoomLevel);
}

- (float)minimumZoomLevel
{
    MGLAssertStyleLayerIsValid();

    return self.rawLayer->getMinZoom();
}

- (void)setTransition:(MGLTransition)transition forKey:(NSString *)key
{
    SEL setPropertyTransitionSelector = [self mbx_selectorForKey:key type:@"set" hasParams:YES];
    
    if ([self respondsToSelector:setPropertyTransitionSelector]) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
        [self performSelector:setPropertyTransitionSelector withObject:[NSValue value:&transition withObjCType:@encode(MGLTransition)] ];
#pragma clang diagnostic pop
    }
}

- (MGLTransition)transitionForKey:(NSString *)key
{
    MGLTransition transition;
    
    SEL getPropertyTransitionSelector = [self mbx_selectorForKey:key type:@"get" hasParams:NO];
    
    if ([self respondsToSelector:getPropertyTransitionSelector]) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
        NSValue *transitionValue = [self performSelector:getPropertyTransitionSelector];
        [transitionValue getValue:&transition];
#pragma clang diagnostic pop
    }
    
    return transition;
}

- (SEL)mbx_selectorForKey:(NSString *)key type:(NSString *)type hasParams:(BOOL)params
{
    NSString *camelCaseKey;
    NSString *parameters = params ? @":" : @"";
    if ([key length] > 1) {
        camelCaseKey = [NSString stringWithFormat:@"%@%@", [[key substringToIndex:1] uppercaseString], [key substringFromIndex:1]];
    } else {
        camelCaseKey = [key uppercaseString];
    }
    NSString *setPropertyTransitionString = [NSString stringWithFormat:@"mbx_%@%@Transition%@", type, camelCaseKey, parameters];
    SEL propertyTransitionSelector = NSSelectorFromString(setPropertyTransitionString);
    
    return propertyTransitionSelector;
}

+ (NSArray *)transitionKeys
{
    // This is overridden by subclasses
    [NSException raise:@"MGLAbstractClassException"
                format:@"MGLStyleLayer is an abstract class"];
    return nil;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@; visible = %@>",
            NSStringFromClass([self class]), (void *)self, self.identifier,
            self.visible ? @"YES" : @"NO"];
}

@end
