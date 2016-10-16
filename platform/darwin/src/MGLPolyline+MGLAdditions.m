#import "MGLPolyline+MGLAdditions.h"

@implementation MGLPolyline (MGLAdditions)

- (NS_ARRAY_OF(id) *)mgl_coordinates {
    NSMutableArray *coordinates = [[NSMutableArray alloc] initWithCapacity:self.pointCount];
    for (NSUInteger index = 0; index < self.pointCount; index++) {
        CLLocationCoordinate2D coordinate = self.coordinates[index];
        [coordinates addObject:@[@(coordinate.longitude), @(coordinate.latitude)]];
    }
    return [coordinates copy];
}

@end
