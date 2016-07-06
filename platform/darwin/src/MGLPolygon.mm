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

- (mbgl::Annotation)annotationObjectWithDelegate:(id <MGLMultiPointDelegate>)delegate {
    mbgl::Polygon<double> geometry;
    geometry.push_back(self.ring);
    for (MGLPolygon *polygon in self.interiorPolygons) {
        geometry.push_back(polygon.ring);
    }

    mbgl::FillAnnotation annotation { geometry };
    annotation.opacity = [delegate alphaForShapeAnnotation:self];
    annotation.outlineColor = [delegate strokeColorForShapeAnnotation:self];
    annotation.color = [delegate fillColorForPolygonAnnotation:self];

    return annotation;
}

- (BOOL)containsPoint:(CGPoint)point transform:(CGPoint (^)(CLLocationCoordinate2D))transform {
    BOOL isInside = NO;
    NSInteger j = self.pointCount - 1;
    for (NSInteger i = 0; i < self.pointCount; i++) {
        CGPoint vertex1 = transform(self.coordinates[i]);
        CGPoint vertex2 = transform(self.coordinates[j]);
        BOOL verticesCrossPoint = (vertex1.y > point.y != vertex2.y > point.y);
        BOOL intersectionPointIsOnRay = (vertex1.x + (point.y - vertex1.y) / (vertex2.y - vertex1.y) * (vertex2.x - vertex1.x)) < point.x;
        if (verticesCrossPoint && intersectionPointIsOnRay) {
            isInside = !isInside;
        }
        j = i;
    }
    return isInside;
}

- (BOOL)isWithinDistance:(CGFloat)distance ofPoint:(CGPoint)point transform:(CGPoint (^)(CLLocationCoordinate2D))transform {
    return [self containsPoint:point transform:transform] || [super isWithinDistance:distance ofPoint:point transform:transform];
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

- (BOOL)isWithinDistance:(CGFloat)distance ofPoint:(CGPoint)point transform:(CGPoint (^)(CLLocationCoordinate2D))transform {
    for (MGLPolygon *polygon in _polygons) {
        if ([polygon isWithinDistance:distance ofPoint:point transform:transform]) {
            return YES;
        }
    }
    return NO;
}

@end
