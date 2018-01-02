#import "MGLPolyline_Private.h"

#import "MGLMultiPoint_Private.h"
#import "MGLGeometry_Private.h"

#import "MGLFeature.h"

#import <mbgl/util/geojson.hpp>
#import <mapbox/polylabel.hpp>

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

- (mbgl::Geometry<double>)geometryObject {
    return [self lineString];
}

- (NSDictionary *)geoJSONDictionary {
    return @{@"type": @"LineString",
             @"coordinates": self.mgl_coordinates};
}

- (NS_ARRAY_OF(id) *)mgl_coordinates {
    NSMutableArray *coordinates = [[NSMutableArray alloc] initWithCapacity:self.pointCount];
    for (NSUInteger index = 0; index < self.pointCount; index++) {
        CLLocationCoordinate2D coordinate = self.coordinates[index];
        [coordinates addObject:@[@(coordinate.longitude), @(coordinate.latitude)]];
    }
    return [coordinates copy];
}

- (BOOL)isEqual:(id)other {
    return self == other || ([other isKindOfClass:[MGLPolyline class]] && [super isEqual:other]);
}

- (CLLocationCoordinate2D)coordinate {
    NSUInteger count = self.pointCount;
    NSAssert(count > 0, @"Polyline must have coordinates");

    CLLocationCoordinate2D *coordinates = self.coordinates;
    CLLocationDistance middle = [self length] / 2.0;
    CLLocationDistance traveled = 0.0;
    
    if (count > 1 || middle > traveled) {
        for (NSUInteger i = 0; i < count; i++) {
            
            MGLRadianCoordinate2D from = MGLRadianCoordinateFromLocationCoordinate(coordinates[i]);
            MGLRadianCoordinate2D to = MGLRadianCoordinateFromLocationCoordinate(coordinates[i + 1]);
            
            if (traveled >= middle) {
                double overshoot = middle - traveled;
                if (overshoot == 0) {
                    return coordinates[i];
                }
                to = MGLRadianCoordinateFromLocationCoordinate(coordinates[i - 1]);
                CLLocationDirection direction = [self direction:from to:to] - 180;
                MGLRadianCoordinate2D otherCoordinate = MGLRadianCoordinateAtDistanceFacingDirection(from,
                                                                                                     overshoot/mbgl::util::EARTH_RADIUS_M,
                                                                                                     MGLRadiansFromDegrees(direction));
                return CLLocationCoordinate2DMake(MGLDegreesFromRadians(otherCoordinate.latitude),
                                                  MGLDegreesFromRadians(otherCoordinate.longitude));
            }
            
            traveled += (MGLDistanceBetweenRadianCoordinates(from, to) * mbgl::util::EARTH_RADIUS_M);
            
        }
    }

    return coordinates[count - 1];
}

- (CLLocationDistance)length
{
    CLLocationDistance length = 0.0;
    
    NSUInteger count = self.pointCount;
    CLLocationCoordinate2D *coordinates = self.coordinates;
    
    for (NSUInteger i = 0; i < count - 1; i++) {        
        length += (MGLDistanceBetweenRadianCoordinates(MGLRadianCoordinateFromLocationCoordinate(coordinates[i]),                                                  MGLRadianCoordinateFromLocationCoordinate(coordinates[i + 1])) * mbgl::util::EARTH_RADIUS_M);
    }
    
    return length;
}

- (CLLocationDirection)direction:(MGLRadianCoordinate2D)from to:(MGLRadianCoordinate2D)to
{
    return MGLDegreesFromRadians(MGLRadianCoordinatesDirection(from, to));
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

- (instancetype)initWithCoder:(NSCoder *)decoder {
    if (self = [super initWithCoder:decoder]) {
        _polylines = [decoder decodeObjectOfClass:[NSArray class] forKey:@"polylines"];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
    [super encodeWithCoder:coder];
    [coder encodeObject:_polylines forKey:@"polylines"];
}

- (BOOL)isEqual:(id)other
{
    if (self == other) return YES;
    if (![other isKindOfClass:[MGLMultiPolyline class]]) return NO;

    MGLMultiPolyline *otherMultipoline = other;
    return ([super isEqual:otherMultipoline]
            && [self.polylines isEqualToArray:otherMultipoline.polylines]);
}

- (NSUInteger)hash {
    NSUInteger hash = [super hash];
    for (MGLPolyline *polyline in self.polylines) {
        hash += [polyline hash];
    }
    return hash;
}

- (CLLocationCoordinate2D)coordinate {
    MGLPolyline *polyline = self.polylines.firstObject;
    CLLocationCoordinate2D *coordinates = polyline.coordinates;
    NSAssert([polyline pointCount] > 0, @"Polyline must have coordinates");
    CLLocationCoordinate2D firstCoordinate = coordinates[0];

    return firstCoordinate;
}

- (BOOL)intersectsOverlayBounds:(MGLCoordinateBounds)overlayBounds {
    return MGLCoordinateBoundsIntersectsCoordinateBounds(_overlayBounds, overlayBounds);
}

- (mbgl::Geometry<double>)geometryObject {
    mbgl::MultiLineString<double> multiLineString;
    multiLineString.reserve(self.polylines.count);
    for (MGLPolyline *polyline in self.polylines) {
        multiLineString.push_back([polyline lineString]);
    }
    return multiLineString;
}

- (NSDictionary *)geoJSONDictionary {
    NSMutableArray *coordinates = [NSMutableArray array];
    for (MGLPolylineFeature *feature in self.polylines) {
        [coordinates addObject: feature.mgl_coordinates];
    }
    return @{@"type": @"MultiLineString",
             @"coordinates": coordinates};
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; title = %@, subtitle: = %@, count = %lu; bounds = %@>",
            NSStringFromClass([self class]), (void *)self,
            self.title ? [NSString stringWithFormat:@"\"%@\"", self.title] : self.title,
            self.subtitle ? [NSString stringWithFormat:@"\"%@\"", self.subtitle] : self.subtitle,
            (unsigned long)self.polylines.count,
            MGLStringFromCoordinateBounds(self.overlayBounds)];
}

@end
