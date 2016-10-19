#import "MGLPointCollection_Private.h"

#import <mbgl/util/geometry.hpp>
#import <mbgl/util/feature.hpp>

NS_ASSUME_NONNULL_BEGIN

@implementation MGLPointCollection
{
    size_t _count;
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
        _coordinates = (CLLocationCoordinate2D *)malloc(count * sizeof(CLLocationCoordinate2D));
        for (NSUInteger i = 0; i < count; i++)
        {
            _coordinates[i] = coords[i];
        }        
        _count = count;
    }
    return self;
}

- (NSUInteger)pointCount
{
    return _count;
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
