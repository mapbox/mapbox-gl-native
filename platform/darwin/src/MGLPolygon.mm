#import "MGLPolygon.h"

#import "MGLMultiPoint_Private.h"
#import "MGLGeometry_Private.h"

#import "MGLPolygon+MGLAdditions.h"

@implementation MGLPolygon

@dynamic overlayBounds;

+ (instancetype)polygonWithCoordinates:(const CLLocationCoordinate2D *)coords count:(NSUInteger)count {
    return [self polygonWithCoordinates:coords count:count interiorPolygons:nil];
}

+ (instancetype)polygonWithCoordinates:(const CLLocationCoordinate2D *)coords count:(NSUInteger)count interiorPolygons:(NSArray<MGLPolygon *> *)interiorPolygons {
    return [[self alloc] initWithCoordinates:coords count:count interiorPolygons:interiorPolygons];
}

- (instancetype)initWithCoordinates:(const CLLocationCoordinate2D *)coords count:(NSUInteger)count interiorPolygons:(NSArray<MGLPolygon *> *)interiorPolygons {
    if (self = [super initWithCoordinates:coords count:count]) {
        if (interiorPolygons.count) {
            _interiorPolygons = interiorPolygons;
        }
    }
    return self;
}

- (mbgl::LinearRing<double>)ring {
    NSUInteger count = self.pointCount;
    CLLocationCoordinate2D *coordinates = self.coordinates;

    mbgl::LinearRing<double> result;
    result.reserve(self.pointCount);
    for (NSUInteger i = 0; i < count; i++) {
        result.push_back(mbgl::Point<double>(coordinates[i].longitude, coordinates[i].latitude));
    }
    return result;
}

- (mbgl::Feature)featureObject {
    mbgl::Polygon<double> geometry;
    geometry.push_back(self.ring);
    for (MGLPolygon *polygon in self.interiorPolygons) {
        geometry.push_back(polygon.ring);
    }
    return mbgl::Feature{geometry};
}

- (mbgl::Annotation)annotationObjectWithDelegate:(id <MGLMultiPointDelegate>)delegate {
    mbgl::Polygon<double> geometry;
    geometry.push_back(self.ring);
    for (MGLPolygon *polygon in self.interiorPolygons) {
        geometry.push_back(polygon.ring);
    }

    mbgl::FillAnnotation annotation { geometry };
    annotation.opacity = { static_cast<float>([delegate alphaForShapeAnnotation:self]) };
    annotation.outlineColor = { [delegate strokeColorForShapeAnnotation:self] };
    annotation.color = { [delegate fillColorForPolygonAnnotation:self] };

    return annotation;
}

- (NSDictionary *)geoJSONDictionary {
    return @{@"type": @"Polygon",
             @"coordinates": self.mgl_coordinates};
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

- (mbgl::Feature)featureObject {
    mbgl::MultiPolygon<double> multiPolygon;
    multiPolygon.reserve(self.polygons.count);
    for (MGLPolygon *polygon in self.polygons) {
        mbgl::Polygon<double> geometry;
        geometry.push_back(polygon.ring);
        for (MGLPolygon *interiorPolygon in polygon.interiorPolygons) {
            geometry.push_back(interiorPolygon.ring);
        }
        multiPolygon.push_back(geometry);
    }
    return mbgl::Feature {multiPolygon};
}

- (NSDictionary *)geoJSONDictionary {
    NSMutableArray *coordinates = [[NSMutableArray alloc] initWithCapacity:self.polygons.count];
    for (MGLPolygonFeature *feature in self.polygons) {
        [coordinates addObject: feature.mgl_coordinates];
    }
    return @{@"type": @"MultiPolygon",
             @"coordinates": coordinates};
}

@end
