#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLMapView.h"
#import "MGLStyleValue.h"
#import "MGLStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

typedef struct MGLOpenGLStyleLayerDrawingState {
    CGSize size;
    CLLocationCoordinate2D centerCoordinate;
    double zoomLevel;
    CLLocationDirection direction;
    CGFloat pitch;
    CGFloat perspectiveSkew;
} MGLOpenGLStyleLayerDrawingState;

typedef void (^MGLOpenGLStyleLayerPreparationHandler)(void);

typedef void (^MGLOpenGLStyleLayerDrawingHandler)(MGLOpenGLStyleLayerDrawingState state);

typedef void (^MGLOpenGLStyleLayerCompletionHandler)(void);

@interface MGLOpenGLStyleLayer : MGLStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier preparationHandler:(MGLOpenGLStyleLayerPreparationHandler)preparation drawingHandler:(MGLOpenGLStyleLayerDrawingHandler)drawing completionHandler:(MGLOpenGLStyleLayerCompletionHandler)completion NS_DESIGNATED_INITIALIZER;

@end

@interface MGLMapView (MGLOpenGLStyleLayerAdditions)

- (void)setStyleLayersNeedDisplay;

@end

NS_ASSUME_NONNULL_END
