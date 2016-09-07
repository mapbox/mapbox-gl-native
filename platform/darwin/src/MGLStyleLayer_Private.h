#import <Foundation/Foundation.h>
#include <mbgl/style/layer.hpp>

#import "MGLStyleLayer.h"
#import "MGLStyleAttribute.h"

#import "NSNumber+MGLStyleAttributeAdditions_Private.h"
#import "NSArray+MGLStyleAttributeAdditions_Private.h"
#import "NSString+MGLStyleAttributeAdditions_Private.h"
#import "NSValue+MGLStyleAttributeAdditions_Private.h"
#import "MGLStyleAttributeFunction_Private.h"
#import "MGLStyleAttributeValue_Private.h"

#if TARGET_OS_IPHONE
    #import "UIColor+MGLAdditions.h"
    #import "UIColor+MGLStyleAttributeAdditions_Private.h"
#else
    #import "NSColor+MGLAdditions.h"
    #import "NSColor+MGLStyleAttributeAdditions_Private.h"
#endif

@class MGLMapView;

@protocol MGLStyleLayer_Private <MGLStyleLayer>

@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic, readwrite, copy) NSString *layerIdentifier;
@property (nonatomic) mbgl::style::Layer *layer;

@end
