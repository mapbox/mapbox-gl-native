#import "MGLStyle.h"

#import "MGLStyleLayer.h"
#import "MGLFillStyleLayer.h"
#import <mbgl/util/default_styles.hpp>
#include <mbgl/mbgl.hpp>

@interface MGLStyle (Private)
@property (nonatomic, weak) MGLMapView *mapView;
@end