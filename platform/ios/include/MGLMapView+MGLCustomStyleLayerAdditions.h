#import "MGLMapView.h"
#import <GLKit/GLKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef void (^MGLCustomStyleLayerPreparationHandler)(void);

typedef void (^MGLCustomStyleLayerDrawingHandler)(CGSize size,
                                                  CLLocationCoordinate2D centerCoordinate,
                                                  double zoomLevel,
                                                  CLLocationDirection direction,
                                                  CGFloat pitch,
                                                  CGFloat perspectiveSkew,
						  GLKMatrix4 projMatrix,
						  GLKMatrix4 nativeMatrix,
						  GLKMatrix4 extrudeMatrix);

typedef void (^MGLCustomStyleLayerCompletionHandler)(void);

@interface MGLMapView (MGLCustomStyleLayerAdditions)

- (void)insertCustomStyleLayerWithIdentifier:(NSString *)identifier preparationHandler:(MGLCustomStyleLayerPreparationHandler)preparation drawingHandler:(MGLCustomStyleLayerDrawingHandler)drawing completionHandler:(MGLCustomStyleLayerCompletionHandler)completion belowStyleLayerWithIdentifier:(nullable NSString *)otherIdentifier;

- (void)removeCustomStyleLayerWithIdentifier:(NSString *)identifier;

- (void)setCustomStyleLayersNeedDisplay;

@end

NS_ASSUME_NONNULL_END
