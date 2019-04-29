#import "MGLCircle.h"

#import "MGLGeometry_Private.h"
#import "MGLMultiPoint_Private.h"
#import "NSCoder+MGLAdditions.h"

#import <mbgl/util/projection.hpp>

#import <vector>

@implementation MGLCircle

@synthesize coordinate = _coordinate;

+ (instancetype)circleWithCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate radius:(CLLocationDistance)radius {
    return [[self alloc] initWithCenterCoordinate:centerCoordinate radius:radius];
}

+ (instancetype)circleWithCoordinateBounds:(MGLCoordinateBounds)coordinateBounds {
    MGLCoordinateSpan span = MGLCoordinateBoundsGetCoordinateSpan(coordinateBounds);
    BOOL latitudinal = span.latitudeDelta > span.longitudeDelta;
    // TODO: Latitudinal distances aren’t uniform, so get the mean northing.
    CLLocationCoordinate2D center = CLLocationCoordinate2DMake(coordinateBounds.ne.latitude - span.latitudeDelta / 2.0,
                                                               coordinateBounds.ne.longitude - span.longitudeDelta / 2.0);
    CLLocationCoordinate2D southOrWest = CLLocationCoordinate2DMake(latitudinal ? coordinateBounds.sw.latitude : 0,
                                                                    latitudinal ? 0 : coordinateBounds.sw.longitude);
    CLLocationCoordinate2D northOrEast = CLLocationCoordinate2DMake(latitudinal ? coordinateBounds.ne.latitude : 0,
                                                                    latitudinal ? 0 : coordinateBounds.ne.longitude);
    CLLocationDistance majorAxis = MGLDistanceBetweenLocationCoordinates(southOrWest, northOrEast);
    return [[self alloc] initWithCenterCoordinate:center radius:majorAxis / 2.0];
}

- (instancetype)initWithCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate radius:(CLLocationDistance)radius {
    if (self = [super init]) {
        _coordinate = centerCoordinate;
        _radius = radius;
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)decoder {
    if (self = [super initWithCoder:decoder]) {
        _coordinate = [decoder decodeMGLCoordinateForKey:@"coordinate"];
        _radius = [decoder decodeDoubleForKey:@"radius"];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
    [super encodeWithCoder:coder];
    [coder encodeMGLCoordinate:_coordinate forKey:@"coordinate"];
    [coder encodeDouble:_radius forKey:@"radius"];
}

- (BOOL)isEqual:(id)other {
    if (other == self) {
        return YES;
    }
    if (![other isKindOfClass:[MGLCircle class]]) {
        return NO;
    }
    
    MGLCircle *otherCircle = other;
    return ([super isEqual:other]
            && self.coordinate.latitude == otherCircle.coordinate.latitude
            && self.coordinate.longitude == otherCircle.coordinate.longitude
            && self.radius == otherCircle.radius);
}

- (NSUInteger)hash {
    return super.hash + @(self.coordinate.latitude).hash + @(self.coordinate.longitude).hash;
}

- (NSUInteger)numberOfVertices {
    // Due to the z16 zoom level and Douglas–Peucker tolerance specified by
    // mbgl::ShapeAnnotationImpl::updateTileData() and GeoJSONVT, the smallest
    // circle that can be displayed at z22 at the poles has a radius of about
    // 5 centimeters and is simplified to four sides each about 0.31 meters
    // (50 points) long. The smallest displayable circle at the Equator has a
    // radius of about 5 decimeters and is simplified to four sides each about
    // 3.1 meters (75 points) long.
    constexpr NSUInteger maximumZoomLevel = 16;
    CLLocationDistance maximumEdgeLength = mbgl::Projection::getMetersPerPixelAtLatitude(self.coordinate.latitude, maximumZoomLevel);
    CLLocationDistance circumference = 2 * M_PI * self.radius;
    NSUInteger maximumSides = ceil(fabs(circumference) / maximumEdgeLength);
    
    // The smallest perceptible angle is about 1 arcminute.
    // https://en.wikipedia.org/wiki/Naked_eye#Small_objects_and_maps
    constexpr CLLocationDirection maximumInternalAngle = 180.0 - 1.0 / 60;
    constexpr CLLocationDirection maximumCentralAngle = 180.0 - maximumInternalAngle;
    constexpr CGFloat maximumVertices = 360.0 / maximumCentralAngle;
    
    // Make the circle’s resolution high enough that the user can’t perceive any
    // angles, but not so high that detail would be lost through simplification.
    return ceil(MIN(maximumSides, maximumVertices));
}

- (mbgl::LinearRing<double>)linearRingWithNumberOfVertices:(NSUInteger)numberOfVertices {
    CLLocationCoordinate2D center = self.coordinate;
    CLLocationDistance radius = fabs(self.radius);
    
    mbgl::LinearRing<double> ring;
    ring.reserve(numberOfVertices);
    for (NSUInteger i = 0; i < numberOfVertices; i++) {
        // Start at due north and go counterclockwise, or phase shift by 90° if
        // centered in the southern hemisphere, so it’s easy to fix up for ±90°
        // latitude in the conditional below.
        CLLocationDirection direction = 360.0 / numberOfVertices * i + (center.latitude >= 0 ? 0 : 180);
        CLLocationCoordinate2D vertex = MGLCoordinateAtDistanceFacingDirection(center, radius, direction);
        // If the circle extends to ±90° latitude and has wrapped around, extend
        // the polygon to include all of ±90° latitude and beyond.
        if (i == 0 && radius > 1
            && fabs(vertex.latitude) < fabs(MGLCoordinateAtDistanceFacingDirection(center, radius - 1, direction).latitude)) {
            short hemisphere = center.latitude >= 0 ? 1 : -1;
            ring.push_back({ center.longitude - 180.0, vertex.latitude });
            ring.push_back({ center.longitude - 180.0, 90.0 * hemisphere });
            ring.push_back({ center.longitude + 180.0, 90.0 * hemisphere });
        }
        ring.push_back(MGLPointFromLocationCoordinate2D(vertex));
    }
    return ring;
}

- (mbgl::Polygon<double>)polygon {
    mbgl::Polygon<double> polygon;
    polygon.push_back([self linearRingWithNumberOfVertices:self.numberOfVertices]);
    return polygon;
}

- (mbgl::Geometry<double>)geometryObject {
    return [self polygon];
}

- (NSDictionary *)geoJSONDictionary {
    return @{
        @"type": @"Polygon",
        @"coordinates": self.geoJSONGeometry,
    };
}

- (NSArray<id> *)geoJSONGeometry {
    NSMutableArray *coordinates = [NSMutableArray array];
    
    mbgl::LinearRing<double> ring = [self polygon][0];
    NSMutableArray *geoJSONRing = [NSMutableArray array];
    for (auto &point : ring) {
        [geoJSONRing addObject:@[@(point.x), @(point.y)]];
    }
    [coordinates addObject:geoJSONRing];
    
    return [coordinates copy];
}

- (mbgl::Annotation)annotationObjectWithDelegate:(id <MGLMultiPointDelegate>)delegate {
    
    mbgl::FillAnnotation annotation { [self polygon] };
    annotation.opacity = { static_cast<float>([delegate alphaForShapeAnnotation:self]) };
    annotation.outlineColor = { [delegate strokeColorForShapeAnnotation:self] };
    annotation.color = { [delegate fillColorForShape:self] };
    
    return annotation;
}

- (MGLCoordinateBounds)coordinateBounds {
    mbgl::LinearRing<double> ring = [self linearRingWithNumberOfVertices:4];
    CLLocationCoordinate2D southWest = CLLocationCoordinate2DMake(ring[2].y, ring[3].x);
    CLLocationCoordinate2D northEast = CLLocationCoordinate2DMake(ring[0].y, ring[1].x);
    return MGLCoordinateBoundsMake(southWest, northEast);
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@: %p; coordinate = %@; radius = %f m>",
            NSStringFromClass([self class]), (void *)self,
            MGLStringFromCLLocationCoordinate2D(self.coordinate), self.radius];
}

@end
