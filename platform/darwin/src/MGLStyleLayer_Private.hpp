#import "MGLStyleLayer.h"

#import "MGLTypes.h"

#include <mbgl/style/layer.hpp>
#include <mbgl/mbgl.hpp>
#include <mbgl/util/chrono.hpp>

@class MGLMapView;

@interface MGLStyleLayer (Private)
@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic) mbgl::Map *mbglMap;
@property (nonatomic) mbgl::style::Layer *layer;

mbgl::Color MGLColorFromColor(MGLColor *color);

- (void)updateStyleAndClasses;

@end
