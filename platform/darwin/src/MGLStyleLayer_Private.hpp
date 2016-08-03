#import <Foundation/Foundation.h>
#include <mbgl/style/layer.hpp>

#import "MGLStyleLayer.h"
#import "MGLStyleAttribute.hpp"

#import "NSNumber+MGLStyleAttributeAdditions_Private.hpp"
#import "NSArray+MGLStyleAttributeAdditions_Private.hpp"
#import "NSString+MGLStyleAttributeAdditions_Private.hpp"
#import "NSValue+MGLStyleAttributeAdditions_Private.hpp"
#import "MGLStyleAttributeFunction_Private.hpp"
#import "MGLStyleAttributeValue_Private.hpp"

#if TARGET_OS_IPHONE
#import "UIColor+MGLAdditions.hpp"
#import "UIColor+MGLStyleAttributeAdditions_Private.hpp"
#else
#import "NSColor+MGLAdditions.hpp"
#import "NSColor+MGLStyleAttributeAdditions_Private.hpp"
#endif

@class MGLMapView;

@protocol MGLStyleLayer_Private <MGLStyleLayer>

@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic, readwrite, copy) NSString *layerIdentifier;
@property (nonatomic) mbgl::style::Layer *layer;

@end
