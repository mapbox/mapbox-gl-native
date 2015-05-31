#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLMultiPoint.h"

@interface MGLPolyline : MGLMultiPoint

+ (instancetype)polylineWithCoordinates:(CLLocationCoordinate2D *)coords
                                  count:(NSUInteger)count;

@end
