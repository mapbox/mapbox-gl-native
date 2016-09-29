//
//  MGLPolyline+MGLPolyline_MGLAdditions.m
//  ios
//
//  Created by Mapbox on 9/30/16.
//  Copyright © 2016 Mapbox. All rights reserved.
//

#import "MGLPolyline+MGLAdditions.h"

@implementation MGLPolyline (MGLAdditions)

- (NS_ARRAY_OF(id) *)mgl_coordinates {
    
    NS_MUTABLE_ARRAY_OF(NS_ARRAY_OF(NSNumber *) *) *coordinates = [NSMutableArray array];
    
    for (NSUInteger index = 0; index < self.pointCount; index++) {
        CLLocationCoordinate2D coordinate = self.coordinates[index];
        [coordinates addObject:@[@(coordinate.longitude), @(coordinate.latitude)]];
    }
    
    return coordinates;
}

@end
