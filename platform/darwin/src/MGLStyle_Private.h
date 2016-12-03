#import "MGLStyle.h"

#import "MGLStyleLayer.h"
#import "MGLFillStyleLayer.h"
#import <mbgl/util/default_styles.hpp>

@interface MGLStyle (Private)

- (instancetype)initWithMapView:(MGLMapView *)mapView;

@property (nonatomic, readonly, weak) MGLMapView *mapView;

- (void)setStyleClasses:(NS_ARRAY_OF(NSString *) *)appliedClasses transitionDuration:(NSTimeInterval)transitionDuration;

@end
