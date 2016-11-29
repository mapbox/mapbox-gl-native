#import "MGLPolyline.h"

#import "MGLMultiPoint_Private.h"
#import "MGLGeometry_Private.h"

#import "MGLPolyline+MGLAdditions.h"

@implementation MGLPolyline

@dynamic overlayBounds;

+ (instancetype)polylineWithCoordinates:(const CLLocationCoordinate2D *)coords
                                  count:(NSUInteger)count
{
    return [[self alloc] initWithCoordinates:coords count:count];
}

- (mbgl::LineString<double>)lineString {
    NSUInteger count = self.pointCount;
    CLLocationCoordinate2D *coordinates = self.coordinates;
    
    mbgl::LineString<double> geometry;
    geometry.reserve(self.pointCount);
    for (NSUInteger i = 0; i < count; i++) {
        geometry.push_back(mbgl::Point<double>(coordinates[i].longitude, coordinates[i].latitude));
    }
    
    return geometry;
}

- (mbgl::Annotation)annotationObjectWithDelegate:(id <MGLMultiPointDelegate>)delegate {
    mbgl::LineAnnotation annotation { [self lineString] };
    annotation.opacity = { static_cast<float>([delegate alphaForShapeAnnotation:self]) };
    annotation.color = { [delegate strokeColorForShapeAnnotation:self] };
    annotation.width = { static_cast<float>([delegate lineWidthForPolylineAnnotation:self]) };

    return annotation;
}

- (mbgl::Feature)featureObject {
    return mbgl::Feature {[self lineString]};    
}

- (NSDictionary *)geoJSONDictionary {
    return @{@"type": @"LineString",
             @"coordinates": self.mgl_coordinates};
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

- (mbgl::Feature)featureObject {
    mbgl::MultiLineString<double> multiLineString;
    multiLineString.reserve(self.polylines.count);
    for (MGLPolyline *polyline in self.polylines) {
        multiLineString.push_back([polyline lineString]);
    }
    return mbgl::Feature {multiLineString};
}

- (NSDictionary *)geoJSONDictionary {
    NSMutableArray *coordinates = [NSMutableArray array];
    for (MGLPolylineFeature *feature in self.polylines) {
        [coordinates addObject: feature.mgl_coordinates];
    }    
    return @{@"type": @"MultiLineString",
             @"coordinates": coordinates};
}

@end
