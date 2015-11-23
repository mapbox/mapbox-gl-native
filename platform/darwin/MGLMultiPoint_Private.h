#import "MGLMultiPoint.h"

#import "MGLGeometry.h"
#import "MGLTypes.h"

#import <mbgl/annotation/shape_annotation.hpp>
#import <vector>

#import <CoreGraphics/CoreGraphics.h>
#import <CoreLocation/CoreLocation.h>

NS_ASSUME_NONNULL_BEGIN

@class MGLPolygon;
@class MGLPolyline;

@protocol MGLMultiPointDelegate;

@interface MGLMultiPoint (Private)

- (instancetype)initWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count;
- (BOOL)intersectsOverlayBounds:(MGLCoordinateBounds)overlayBounds;

- (void)addShapeAnnotationObjectToCollection:(std::vector<mbgl::ShapeAnnotation> &)shapes withDelegate:(id <MGLMultiPointDelegate>)delegate;
- (mbgl::ShapeAnnotation::Properties)shapeAnnotationPropertiesObjectWithDelegate:(id <MGLMultiPointDelegate>)delegate;

@end

@protocol MGLMultiPointDelegate <NSObject>

- (double)alphaForShapeAnnotation:(MGLShape *)annotation;
- (mbgl::Color)strokeColorForShapeAnnotation:(MGLShape *)annotation;
- (mbgl::Color)fillColorForPolygonAnnotation:(MGLPolygon *)annotation;
- (CGFloat)lineWidthForPolylineAnnotation:(MGLPolyline *)annotation;

@end

NS_ASSUME_NONNULL_END
