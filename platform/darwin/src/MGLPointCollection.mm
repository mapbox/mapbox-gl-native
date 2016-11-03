#import "MGLPointCollection_Private.h"
#import "MGLGeometry_Private.h"

#import <mbgl/util/geometry.hpp>
#import <mbgl/util/feature.hpp>

NS_ASSUME_NONNULL_BEGIN

@implementation MGLPointCollection
{
    size_t _count;
    MGLCoordinateBounds _bounds;
}

+ (instancetype)pointCollectionWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count
{
    return [[self alloc] initWithCoordinates:coords count:count];
}

- (instancetype)initWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count
{
    self = [super init];
    if (self)
    {
        _count = count;
        _coordinates = (CLLocationCoordinate2D *)malloc(_count * sizeof(CLLocationCoordinate2D));
        
        mbgl::LatLngBounds bounds = mbgl::LatLngBounds::empty();
        
        for (NSUInteger i = 0; i < count; i++)
        {
            _coordinates[i] = coords[i];
            bounds.extend(mbgl::LatLng(coords[i].latitude, coords[i].longitude));
        }
        
        _bounds = MGLCoordinateBoundsFromLatLngBounds(bounds);
    }
    return self;
}

- (NSUInteger)pointCount
{
    return _count;
}

- (CLLocationCoordinate2D)coordinate
{
    assert(_count > 0);
    
    return CLLocationCoordinate2DMake(_coordinates[0].latitude, _coordinates[0].longitude);
}

- (void)getCoordinates:(CLLocationCoordinate2D *)coords range:(NSRange)range
{
    if (range.location + range.length > _count)
    {
        [NSException raise:NSRangeException
                    format:@"Invalid coordinate range %@ extends beyond current coordinate count of %zu",
         NSStringFromRange(range), _count];
    }
    
    NSUInteger index = 0;
    
    for (NSUInteger i = range.location; i < range.location + range.length; i++)
    {
        coords[index] = _coordinates[i];
        index++;
    }
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
            NSStringFromClass([self class]), (void *)self, (unsigned long)_count];
}

@end

NS_ASSUME_NONNULL_END
