#import "MGLPointCollection_Private.h"
#import "MGLGeometry_Private.h"

#import <mbgl/util/geometry.hpp>
#import <mbgl/util/feature.hpp>

NS_ASSUME_NONNULL_BEGIN

@implementation MGLPointCollection
{
    MGLCoordinateBounds _bounds;
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
        mbgl::LatLngBounds bounds = mbgl::LatLngBounds::empty();
        for (auto coordinate : _coordinates)
        {
            bounds.extend(mbgl::LatLng(coordinate.latitude, coordinate.longitude));
        }
        _bounds = MGLCoordinateBoundsFromLatLngBounds(bounds);
    }
    return self;
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

- (MGLCoordinateBounds)overlayBounds
{
    return _bounds;
}

- (BOOL)intersectsOverlayBounds:(MGLCoordinateBounds)overlayBounds
{
    return MGLLatLngBoundsFromCoordinateBounds(_bounds).intersects(MGLLatLngBoundsFromCoordinateBounds(overlayBounds));
}

- (mbgl::Feature)featureObject
{
    mbgl::MultiPoint<double> multiPoint;
    multiPoint.reserve(self.pointCount);
    for (NSInteger i = 0; i< self.pointCount; i++)
    {
        multiPoint.push_back(mbgl::Point<double>(self.coordinates[i].longitude, self.coordinates[i].latitude));
    }
    return mbgl::Feature {multiPoint};
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
    return [NSString stringWithFormat:@"<%@: %p; count = %lu>",
            NSStringFromClass([self class]), (void *)self, (unsigned long)[self pointCount]];
}

@end

NS_ASSUME_NONNULL_END
