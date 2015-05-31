#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLShape.h"

@interface MGLPointAnnotation : MGLShape

@property (nonatomic, assign) CLLocationCoordinate2D coordinate;

@end
