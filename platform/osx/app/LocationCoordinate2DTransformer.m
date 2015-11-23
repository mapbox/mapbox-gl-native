#import "LocationCoordinate2DTransformer.h"

#import "NSValue+Additions.h"

NSString *StringFromDegrees(CLLocationDegrees degrees, char positiveDirection, char negativeDirection) {
    double minutes = (degrees - floor(degrees)) * 60;
    double seconds = (minutes - floor(minutes)) * 60;
    
    NSMutableString *string = [NSMutableString stringWithFormat:@"%.0f°", fabs(degrees)];
    if (floor(minutes) || floor(seconds)) {
        [string appendFormat:@"%.0f′", minutes];
    }
    if (floor(seconds)) {
        [string appendFormat:@"%.0f″", seconds];
    }
    if (degrees) {
        [string appendFormat:@"%c", degrees > 0 ? positiveDirection : negativeDirection];
    }
    return string;
}

@implementation LocationCoordinate2DTransformer

+ (Class)transformedValueClass {
    return [NSString class];
}

+ (BOOL)allowsReverseTransformation {
    return NO;
}

- (id)transformedValue:(id)value {
    if (![value isKindOfClass:[NSValue class]]) {
        return nil;
    }
    CLLocationCoordinate2D coordinate = [value CLLocationCoordinate2DValue];
    return [NSString stringWithFormat:@"%@, %@",
            StringFromDegrees(coordinate.latitude, 'N', 'S'),
            StringFromDegrees(coordinate.longitude, 'E', 'W')];
}

@end
