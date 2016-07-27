#import "MGLStyleLayer.h"

#import "MGLTypes.h"
#import "MGLStyleAttributeValue.h"

#import "NSNumber+MGLStyleAttributeAdditions_Private.hpp"
#import "NSArray+MGLStyleAttributeAdditions_Private.hpp"
#import "NSString+MGLStyleAttributeAdditions_Private.hpp"

#include <mbgl/style/layer.hpp>
#include <mbgl/mbgl.hpp>
#include <mbgl/util/chrono.hpp>

#if TARGET_OS_IPHONE
#import "UIColor+MGLAdditions.hpp"
#else
#import "NSColor+MGLAdditions.hpp"
#endif

@class MGLMapView;

@interface MGLStyleLayer (Private)

@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic) mbgl::style::Layer *layer;

- (void)updateStyleAndClasses;

@end
