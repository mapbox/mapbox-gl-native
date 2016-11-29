#import "MGLMultiPoint.h"

#import "MGLGeometry.h"

#import <mbgl/annotation/annotation.hpp>
#import <mbgl/util/feature.hpp>
#import <vector>

#import <CoreGraphics/CoreGraphics.h>
#import <CoreLocation/CoreLocation.h>

NS_ASSUME_NONNULL_BEGIN

@class MGLPolygon;
@class MGLPolyline;

@protocol MGLMultiPointDelegate;

@interface MGLMultiPoint (Private)

- (instancetype)initWithCoordinates:(const CLLocationCoordinate2D *)coords count:(NSUInteger)count;
- (BOOL)intersectsOverlayBounds:(MGLCoordinateBounds)overlayBounds;

/** Constructs a shape annotation object, asking the delegate for style values. */
- (mbgl::Annotation)annotationObjectWithDelegate:(id <MGLMultiPointDelegate>)delegate;

@end

/** An object that tells the MGLMultiPoint instance how to style itself. */
@protocol MGLMultiPointDelegate <NSObject>

/** Returns the fill alpha value for the given annotation. */
- (double)alphaForShapeAnnotation:(MGLShape *)annotation;

/** Returns the stroke color object for the given annotation. */
- (mbgl::Color)strokeColorForShapeAnnotation:(MGLShape *)annotation;

/** Returns the fill color object for the given annotation. */
- (mbgl::Color)fillColorForPolygonAnnotation:(MGLPolygon *)annotation;

/** Returns the stroke width object for the given annotation. */
- (CGFloat)lineWidthForPolylineAnnotation:(MGLPolyline *)annotation;

@end

NS_ASSUME_NONNULL_END
