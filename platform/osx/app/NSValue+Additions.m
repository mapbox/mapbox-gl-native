#import "NSValue+Additions.h"

@implementation NSValue (Additions)

+ (instancetype)valueWithCLLocationCoordinate2D:(CLLocationCoordinate2D)coordinate {
    return [self valueWithBytes:&coordinate objCType:@encode(CLLocationCoordinate2D)];
}

- (CLLocationCoordinate2D)CLLocationCoordinate2DValue {
    CLLocationCoordinate2D coordinate;
    [self getValue:&coordinate];
    return coordinate;
}

@end
