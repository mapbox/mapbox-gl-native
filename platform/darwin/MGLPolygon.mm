#import "MGLPolygon.h"

#import "MGLMultiPoint_Private.h"

@implementation MGLPolygon

@dynamic overlayBounds;

+ (instancetype)polygonWithCoordinates:(CLLocationCoordinate2D *)coords
                                 count:(NSUInteger)count
{
    return [[self alloc] initWithCoordinates:coords count:count];
}

- (mbgl::ShapeAnnotation::Properties)shapeAnnotationPropertiesObjectWithDelegate:(id <MGLMultiPointDelegate>)delegate {
    mbgl::ShapeAnnotation::Properties shapeProperties = [super shapeAnnotationPropertiesObjectWithDelegate:delegate];
    
    mbgl::FillAnnotationProperties fillProperties;
    fillProperties.opacity = [delegate alphaForShapeAnnotation:self];
    fillProperties.outlineColor = [delegate strokeColorForShapeAnnotation:self];
    fillProperties.color = [delegate fillColorForPolygonAnnotation:self];
    
    shapeProperties.set<mbgl::FillAnnotationProperties>(fillProperties);
    
    return shapeProperties;
}

@end
