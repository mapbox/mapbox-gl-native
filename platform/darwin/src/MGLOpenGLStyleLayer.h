#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
#import <QuartzCore/QuartzCore.h>

#import "MGLFoundation.h"
#import "MGLStyleValue.h"
#import "MGLStyleLayer.h"
#import "MGLGeometry.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLMapView;
@class MGLStyle;

typedef struct MGLStyleLayerDrawingContext {
    CGSize size;
    CLLocationCoordinate2D centerCoordinate;
    double zoomLevel;
    CLLocationDirection direction;
    CGFloat pitch;
    CGFloat fieldOfView;
    MGLMatrix4 projectionMatrix;
} MGLStyleLayerDrawingContext;

MGL_EXPORT
@interface MGLOpenGLStyleLayer : MGLStyleLayer

@property (nonatomic, weak, readonly) MGLStyle *style;

- (instancetype)initWithIdentifier:(NSString *)identifier;

- (void)didMoveToMapView:(MGLMapView *)mapView;

- (void)willMoveFromMapView:(MGLMapView *)mapView;

- (void)drawInMapView:(MGLMapView *)mapView withContext:(MGLStyleLayerDrawingContext)context;

- (void)setNeedsDisplay;

@end

NS_ASSUME_NONNULL_END
