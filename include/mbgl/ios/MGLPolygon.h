#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLMultiPoint.h"
#import "MGLOverlay.h"

@interface MGLPolygon : MGLMultiPoint <MGLOverlay>

+ (instancetype)polygonWithCoordinates:(CLLocationCoordinate2D *)coords
                                 count:(NSUInteger)count;

//+ (instancetype)polygonWithCoordinates:(CLLocationCoordinate2D *)coords
//                                 count:(NSUInteger)count
//                      interiorPolygons:(NSArray *)interiorPolygons;

@end
