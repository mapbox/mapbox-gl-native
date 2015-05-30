#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLShape.h"

@interface MGLMultiPoint: MGLShape

- (void)getCoordinates:(CLLocationCoordinate2D *)coords range:(NSRange)range;

@end
