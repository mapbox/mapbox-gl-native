#import <Foundation/Foundation.h>

#import "MGLStyleLayer.h"
#import "MGLStyleValue_Private.h"

#include <mbgl/style/layer.hpp>

@class MGLMapView;

@interface MGLStyleLayer (Private)

@property (nonatomic, readwrite, copy) NSString *identifier;

/**
 A raw pointer to the mbgl object, which is always initialized, either to the
 value returned by `mbgl::Map getLayer`, or for independently created objects,
 to the pointer value held in `pendingLayer`. In the latter case, this raw
 pointer value stays even after ownership of the object is transferred via
 `mbgl::Map addLayer`.
 */
@property (nonatomic) mbgl::style::Layer *rawLayer;

/**
 Adds the mbgl style layer that this object represents to the mbgl map.
 
 Once a mbgl style layer is added, ownership of the object is transferred to the
 `mbgl::Map` and this object no longer has an active unique_ptr reference to the
 `mbgl::style::Layer`.
 */
- (void)addToMapView:(MGLMapView *)mapView;



/**
 Adds the mbgl style layer that this object represents to the mbgl map below the specified `otherLayer`.
 
 Once a mbgl style layer is added, ownership of the object is transferred to the
 `mbgl::Map` and this object no longer has an active unique_ptr reference to the
 `mbgl::style::Layer`.
 */
- (void)addToMapView:(MGLMapView *)mapView belowLayer:(MGLStyleLayer *)otherLayer;

@end
