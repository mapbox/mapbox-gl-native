#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLStyleValue.h"
#import "MGLStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLMapView;

typedef struct MGLStyleLayerDrawingContext {
    CGSize size;
    CLLocationCoordinate2D centerCoordinate;
    double zoomLevel;
    CLLocationDirection direction;
    CGFloat pitch;
} MGLStyleLayerDrawingContext;

@interface MGLOpenGLStyleLayer : MGLStyleLayer

@property (nonatomic, weak, readonly) MGLMapView *mapView;

- (void)didMoveToMapView:(MGLMapView *)mapView;

- (void)willMoveFromMapView:(MGLMapView *)mapView;

- (void)drawInMapView:(MGLMapView *)mapView withContext:(MGLStyleLayerDrawingContext)context;

- (void)setNeedsDisplay;

@end

NS_ASSUME_NONNULL_END
