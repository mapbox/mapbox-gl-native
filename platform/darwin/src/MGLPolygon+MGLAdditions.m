//
//  MGLPolygonFeature+MGLAdditions.m
//  ios
//
//  Created by Mapbox on 9/30/16.
//  Copyright © 2016 Mapbox. All rights reserved.
//

#import "MGLPolygon+MGLAdditions.h"

@implementation MGLPolygon (MGLAdditions)

- (NS_ARRAY_OF(id) *)mgl_coordinates {
    NS_MUTABLE_ARRAY_OF(NS_MUTABLE_ARRAY_OF(NS_ARRAY_OF(NSNumber *) *) *) *coordinates = [NSMutableArray array];
    
    NS_MUTABLE_ARRAY_OF(NS_ARRAY_OF(NSNumber *) *) *exteriorRing = [NSMutableArray array];
    for (NSUInteger index = 0; index < self.pointCount; index++) {
        CLLocationCoordinate2D coordinate = self.coordinates[index];
        [exteriorRing addObject:@[@(coordinate.longitude), @(coordinate.latitude)]];
    }
    [coordinates addObject:exteriorRing];
    
    for (MGLPolygon *interiorPolygon in self.interiorPolygons) {
        NS_MUTABLE_ARRAY_OF(NS_ARRAY_OF(NSNumber *) *) *interiorRing = [NSMutableArray array];
        for (int index = 0; index < interiorPolygon.pointCount; index++) {
            CLLocationCoordinate2D coordinate = interiorPolygon.coordinates[index];
            [interiorRing addObject:@[@(coordinate.longitude), @(coordinate.latitude)]];
        }
        [coordinates addObject:interiorRing];
    }
    
    return coordinates;
}

@end
