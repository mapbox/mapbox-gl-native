#import "MGLPolygon.h"

#import "MGLMultiPoint_Private.h"
#import "MGLGeometry_Private.h"

@implementation MGLPolygon

@dynamic overlayBounds;

+ (instancetype)polygonWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count {
    return [self polygonWithCoordinates:coords count:count interiorPolygons:nil];
}

+ (instancetype)polygonWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count interiorPolygons:(NSArray<MGLPolygon *> *)interiorPolygons {
    return [[self alloc] initWithCoordinates:coords count:count interiorPolygons:interiorPolygons];
}

- (instancetype)initWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count interiorPolygons:(NSArray<MGLPolygon *> *)interiorPolygons {
    if (self = [super initWithCoordinates:coords count:count]) {
        if (interiorPolygons.count) {
            _interiorPolygons = interiorPolygons;
        }
    }
    return self;
}

- (mbgl::AnnotationSegments)annotationSegments {
    auto segments = super.annotationSegments;
    for (MGLPolygon *polygon in self.interiorPolygons) {
        auto interiorSegments = polygon.annotationSegments;
        segments.push_back(interiorSegments.front());
    }
    return segments;
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

@interface MGLMultiPolygon ()

@property (nonatomic, copy, readwrite) NS_ARRAY_OF(MGLPolygon *) *polygons;

@end

@implementation MGLMultiPolygon {
    MGLCoordinateBounds _overlayBounds;
}

@synthesize overlayBounds = _overlayBounds;

+ (instancetype)multiPolygonWithPolygons:(NS_ARRAY_OF(MGLPolygon *) *)polygons {
    return [[self alloc] initWithPolygons:polygons];
}

- (instancetype)initWithPolygons:(NS_ARRAY_OF(MGLPolygon *) *)polygons {
    if (self = [super init]) {
        _polygons = polygons;
        
        mbgl::LatLngBounds bounds = mbgl::LatLngBounds::empty();
        
        for (MGLPolygon *polygon in _polygons) {
            bounds.extend(MGLLatLngBoundsFromCoordinateBounds(polygon.overlayBounds));
        }
        _overlayBounds = MGLCoordinateBoundsFromLatLngBounds(bounds);
    }
    return self;
}

- (BOOL)intersectsOverlayBounds:(MGLCoordinateBounds)overlayBounds {
    return MGLLatLngBoundsFromCoordinateBounds(_overlayBounds).intersects(MGLLatLngBoundsFromCoordinateBounds(overlayBounds));
}

@end
