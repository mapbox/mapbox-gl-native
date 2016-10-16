#import "MGLPolygon+MGLAdditions.h"

@implementation MGLPolygon (MGLAdditions)

- (NS_ARRAY_OF(id) *)mgl_coordinates {
    NSMutableArray *coordinates = [NSMutableArray array];
    
    NSMutableArray *exteriorRing = [NSMutableArray array];
    for (NSUInteger index = 0; index < self.pointCount; index++) {
        CLLocationCoordinate2D coordinate = self.coordinates[index];
        [exteriorRing addObject:@[@(coordinate.longitude), @(coordinate.latitude)]];
    }
    [coordinates addObject:exteriorRing];
    
    for (MGLPolygon *interiorPolygon in self.interiorPolygons) {
        NSMutableArray *interiorRing = [NSMutableArray array];
        for (int index = 0; index < interiorPolygon.pointCount; index++) {
            CLLocationCoordinate2D coordinate = interiorPolygon.coordinates[index];
            [interiorRing addObject:@[@(coordinate.longitude), @(coordinate.latitude)]];
        }
        [coordinates addObject:interiorRing];
    }
    
    return [coordinates copy];
}

@end
