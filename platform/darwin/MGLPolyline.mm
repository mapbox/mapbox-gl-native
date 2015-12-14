#import "MGLPolyline.h"

#import "MGLMultiPoint_Private.h"

@implementation MGLPolyline

@dynamic overlayBounds;

+ (instancetype)polylineWithCoordinates:(CLLocationCoordinate2D *)coords
                                  count:(NSUInteger)count
{
    return [[self alloc] initWithCoordinates:coords count:count];
}

- (mbgl::ShapeAnnotation::Properties)shapeAnnotationPropertiesObjectWithDelegate:(id <MGLMultiPointDelegate>)delegate {
    mbgl::ShapeAnnotation::Properties shapeProperties = [super shapeAnnotationPropertiesObjectWithDelegate:delegate];
    
    mbgl::LineAnnotationProperties lineProperties;
    lineProperties.opacity = [delegate alphaForShapeAnnotation:self];
    lineProperties.color = [delegate strokeColorForShapeAnnotation:self];
    lineProperties.width = [delegate lineWidthForPolylineAnnotation:self];
    
    shapeProperties.set<mbgl::LineAnnotationProperties>(lineProperties);
    
    return shapeProperties;
}

@end
