#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLMultiPoint.h"

@interface MGLPolygon : MGLMultiPoint

+ (instancetype)polygonWithCoordinates:(CLLocationCoordinate2D *)coords
                                 count:(NSUInteger)count;

//+ (instancetype)polygonWithCoordinates:(CLLocationCoordinate2D *)coords
//                                 count:(NSUInteger)count
//                      interiorPolygons:(NSArray *)interiorPolygons;

@end
