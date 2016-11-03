#import <Foundation/Foundation.h>

#import "MGLStyleLayer.h"
#import "MGLStyleValue_Private.h"

#include <mbgl/style/layer.hpp>

@class MGLMapView;

@interface MGLStyleLayer (Private)

@property (nonatomic, readwrite, copy) NSString *identifier;
@property (nonatomic) mbgl::style::Layer *layer;

/**
 Adds the mbgl style layer that this object represents to the mbgl map.
 
 Once a mbgl style layer is added, ownership of the object is transferred to the
 `mbgl::Map` and this object no longer has an active unique_ptr reference to the
 `mbgl::style::Layer`.
 */
- (void)addToMapView:(MGLMapView *)mapView;

@end
