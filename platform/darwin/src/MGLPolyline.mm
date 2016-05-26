#import "MGLPolyline.h"

#import "MGLMultiPoint_Private.h"
#import "MGLGeometry_Private.h"

@implementation MGLPolyline

@dynamic overlayBounds;

+ (instancetype)polylineWithCoordinates:(CLLocationCoordinate2D *)coords
                                  count:(NSUInteger)count
{
    return [[self alloc] initWithCoordinates:coords count:count];
}

- (mbgl::ShapeAnnotation)shapeAnnotationObjectWithDelegate:(id <MGLMultiPointDelegate>)delegate {
    mbgl::LineAnnotationProperties lineProperties;
    lineProperties.opacity = [delegate alphaForShapeAnnotation:self];
    lineProperties.color = [delegate strokeColorForShapeAnnotation:self];
    lineProperties.width = [delegate lineWidthForPolylineAnnotation:self];

    NSUInteger count = self.pointCount;
    CLLocationCoordinate2D *coordinates = self.coordinates;

    mbgl::LineString<double> geometry;
    geometry.reserve(self.pointCount);
    for (NSUInteger i = 0; i < count; i++) {
        geometry.push_back(mbgl::Point<double>(coordinates[i].longitude, coordinates[i].latitude));
    }

    return mbgl::ShapeAnnotation(geometry, lineProperties);
}

@end

@interface MGLMultiPolyline ()

@property (nonatomic, copy, readwrite) NS_ARRAY_OF(MGLPolyline *) *polylines;

@end

@implementation MGLMultiPolyline {
    MGLCoordinateBounds _overlayBounds;
}

@synthesize overlayBounds = _overlayBounds;

+ (instancetype)multiPolylineWithPolylines:(NS_ARRAY_OF(MGLPolyline *) *)polylines {
    return [[self alloc] initWithPolylines:polylines];
}

- (instancetype)initWithPolylines:(NS_ARRAY_OF(MGLPolyline *) *)polylines {
    if (self = [super init]) {
        _polylines = polylines;
        
        mbgl::LatLngBounds bounds = mbgl::LatLngBounds::empty();
        
        for (MGLPolyline *polyline in _polylines) {
            bounds.extend(MGLLatLngBoundsFromCoordinateBounds(polyline.overlayBounds));
        }
        _overlayBounds = MGLCoordinateBoundsFromLatLngBounds(bounds);
    }
    return self;
}

- (BOOL)intersectsOverlayBounds:(MGLCoordinateBounds)overlayBounds {
    return MGLLatLngBoundsFromCoordinateBounds(_overlayBounds).intersects(MGLLatLngBoundsFromCoordinateBounds(overlayBounds));
}

@end
