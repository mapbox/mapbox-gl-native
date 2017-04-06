#import "MGLPointCollection_Private.h"
#import "MGLGeometry_Private.h"
#import "NSArray+MGLAdditions.h"

#import <mbgl/util/geojson.hpp>
#import <mbgl/util/geometry.hpp>

NS_ASSUME_NONNULL_BEGIN

@implementation MGLPointCollection
{
    mbgl::optional<mbgl::LatLngBounds> _bounds;
    std::vector<CLLocationCoordinate2D> _coordinates;
}

+ (instancetype)pointCollectionWithCoordinates:(const CLLocationCoordinate2D *)coords count:(NSUInteger)count
{
    return [[self alloc] initWithCoordinates:coords count:count];
}

- (instancetype)initWithCoordinates:(const CLLocationCoordinate2D *)coords count:(NSUInteger)count
{
    self = [super init];
    if (self)
    {
        _coordinates = { coords, coords + count };
    }
    return self;
}

- (nullable instancetype)initWithCoder:(NSCoder *)decoder {
    if (self = [super initWithCoder:decoder]) {
        NSArray *coordinates = [decoder decodeObjectOfClass:[NSArray class] forKey:@"coordinates"];
        _coordinates = [coordinates mgl_coordinates];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
    [super encodeWithCoder:coder];
    [coder encodeObject:[NSArray mgl_coordinatesFromCoordinates:_coordinates] forKey:@"coordinates"];
}

- (BOOL)isEqual:(id)other {
    if (self == other) return YES;
    if (![other isKindOfClass:[MGLPointCollection class]]) return NO;

    MGLPointCollection *otherCollection = (MGLPointCollection *)other;
    return ([super isEqual:other]
            && ((![self geoJSONDictionary] && ![otherCollection geoJSONDictionary]) || [[self geoJSONDictionary] isEqualToDictionary:[otherCollection geoJSONDictionary]]));
}

- (MGLCoordinateBounds)overlayBounds {
    if (!_bounds) {
        mbgl::LatLngBounds bounds = mbgl::LatLngBounds::empty();
        for (auto coordinate : _coordinates) {
            if (!CLLocationCoordinate2DIsValid(coordinate)) {
                bounds = mbgl::LatLngBounds::empty();
                break;
            }
            bounds.extend(MGLLatLngFromLocationCoordinate2D(coordinate));
        }
        _bounds = bounds;
    }
    return MGLCoordinateBoundsFromLatLngBounds(*_bounds);
}

- (NSUInteger)pointCount
{
    return _coordinates.size();
}

- (CLLocationCoordinate2D *)coordinates
{
    return _coordinates.data();
}

- (CLLocationCoordinate2D)coordinate
{
    NSAssert([self pointCount] > 0, @"A multipoint must have coordinates");
    return _coordinates.at(0);
}

- (void)getCoordinates:(CLLocationCoordinate2D *)coords range:(NSRange)range
{
    if (range.location + range.length > [self pointCount])
    {
        [NSException raise:NSRangeException
                    format:@"Invalid coordinate range %@ extends beyond current coordinate count of %ld",
         NSStringFromRange(range), (unsigned long)[self pointCount]];
    }

    std::copy(_coordinates.begin() + range.location, _coordinates.begin() + NSMaxRange(range), coords);
}

- (BOOL)intersectsOverlayBounds:(MGLCoordinateBounds)overlayBounds
{
    return MGLCoordinateBoundsIntersectsCoordinateBounds(self.overlayBounds, overlayBounds);
}

- (mbgl::Geometry<double>)geometryObject
{
    mbgl::MultiPoint<double> multiPoint;
    multiPoint.reserve(self.pointCount);
    for (NSInteger i = 0; i< self.pointCount; i++)
    {
        multiPoint.push_back(mbgl::Point<double>(self.coordinates[i].longitude, self.coordinates[i].latitude));
    }
    return multiPoint;
}

- (NSDictionary *)geoJSONDictionary
{
    NSMutableArray *coordinates = [[NSMutableArray alloc] initWithCapacity:self.pointCount];
    for (NSUInteger index = 0; index < self.pointCount; index++) {
        CLLocationCoordinate2D coordinate = self.coordinates[index];
        [coordinates addObject:@[@(coordinate.longitude), @(coordinate.latitude)]];
    }

    return @{@"type": @"MultiPoint",
             @"coordinates": coordinates};
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; count = %lu; bounds = %@>",
            NSStringFromClass([self class]), (void *)self, (unsigned long)[self pointCount],
            MGLStringFromCoordinateBounds(self.overlayBounds)];
}

@end

NS_ASSUME_NONNULL_END
