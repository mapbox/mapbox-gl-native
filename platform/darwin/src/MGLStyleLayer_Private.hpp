#import <Foundation/Foundation.h>
#import "MGLStyleLayer.h"
#include <mbgl/style/layer.hpp>

#import "NSNumber+MGLStyleAttributeAdditions_Private.hpp"
#import "NSArray+MGLStyleAttributeAdditions_Private.hpp"
#import "NSString+MGLStyleAttributeAdditions_Private.hpp"
#import "NSValue+MGLStyleAttributeAdditions_Private.hpp"
#import "MGLStyleAttributeFunction_Private.hpp"
#import "MGLStyleAttributeValue_Private.hpp"

#if TARGET_OS_IPHONE
#import "UIColor+MGLAdditions.hpp"
#else
#import "NSColor+MGLAdditions.hpp"
#endif

@class MGLMapView;

@protocol MGLStyleLayer_Private <MGLStyleLayer>

@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic, readwrite, copy) NSString *layerIdentifier;
@property (nonatomic) mbgl::style::Layer *layer;

@end
