#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

@interface NSValue (Additions)

+ (instancetype)valueWithCLLocationCoordinate2D:(CLLocationCoordinate2D)coordinate;

@property (readonly) CLLocationCoordinate2D CLLocationCoordinate2DValue;

@end
