#import "MGLCoordinateFormatter.h"

#import "NSValue+MGLAdditions.h"

@implementation MGLCoordinateFormatter {
    NSNumberFormatter *_numberFormatter;
}

- (instancetype)init {
    if (self = [super init]) {
        _numberFormatter = [[NSNumberFormatter alloc] init];
        _numberFormatter.numberStyle = NSNumberFormatterDecimalStyle;
        _numberFormatter.maximumFractionDigits = 0;
    }
    return self;
}

- (NSString *)stringFromCoordinate:(CLLocationCoordinate2D)coordinate {
    return [NSString stringWithFormat:NSLocalizedString(@"%@, %@", @"Latitude, longitude format"),
            [self stringFromLocationDegrees:coordinate.latitude
                             positiveFormat:NSLocalizedString(@"%@N", @"North latitude format")
                             negativeFormat:NSLocalizedString(@"%@S", @"South latitude format")],
            [self stringFromLocationDegrees:coordinate.longitude
                             positiveFormat:NSLocalizedString(@"%@E", @"East longitude format")
                             negativeFormat:NSLocalizedString(@"%@W", @"West longitude format")]];
}

- (NSString *)stringFromLocationDegrees:(CLLocationDegrees)degrees positiveFormat:(NSString *)positiveFormat negativeFormat:(NSString *)negativeFormat {
    CLLocationDegrees minutes = (fabs(degrees) - floor(fabs(degrees))) * 60;
    CLLocationDegrees seconds = (minutes - floor(minutes)) * 60;
    
    NSMutableString *string = [NSMutableString stringWithFormat:NSLocalizedString(@"%@°", @"Degrees of arc format"),
                               [_numberFormatter stringFromNumber:@(floor(fabs(degrees)))]];
    if (trunc(minutes) > 0 || trunc(seconds) > 0) {
        [string appendFormat:NSLocalizedString(@"%@′", @"Arcminutes format"),
         [_numberFormatter stringFromNumber:@(floor(minutes))]];
    }
    if (trunc(seconds) > 0) {
        [string appendFormat:NSLocalizedString(@"%@″", @"Arcseconds format"),
         [_numberFormatter stringFromNumber:@(seconds)]];
    }
    if (degrees == 0) {
        return string;
    }
    return [NSString stringWithFormat:degrees > 0 ? positiveFormat : negativeFormat, string];
}

- (nullable NSString *)stringForObjectValue:(id)obj {
    if (![obj isKindOfClass:[NSValue class]]) {
        return nil;
    }
    return [self stringFromCoordinate:[obj MGLCoordinateValue]];
}

- (BOOL)getObjectValue:(out id __nullable * __nullable)obj forString:(NSString *)string errorDescription:(out NSString * __nullable * __nullable)error {
    NSAssert(NO, @"-getObjectValue:forString:errorDescription: has not been implemented");
    return NO;
}

@end
