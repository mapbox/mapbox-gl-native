#import "MGLCoordinateFormatter.h"

#import "NSBundle+MGLAdditions.h"
#import "NSValue+MGLAdditions.h"

@implementation MGLCoordinateFormatter

- (instancetype)init {
    if (self = [super init]) {
        _allowsMinutes = YES;
        _allowsSeconds = YES;
        _unitStyle = NSFormattingUnitStyleMedium;
    }
    return self;
}

- (NSString *)stringFromCoordinate:(CLLocationCoordinate2D)coordinate {
    NSString *positiveLatitudeFormat;
    NSString *negativeLatitudeFormat;
    NSString *positiveLongitudeFormat;
    NSString *negativeLongitudeFormat;
    NSString *stringFormat;
    switch (self.unitStyle) {
        case NSFormattingUnitStyleShort:
            positiveLatitudeFormat = NSLocalizedStringFromTable(@"%@N", @"Foundation", @"North latitude format, short");
            negativeLatitudeFormat = NSLocalizedStringFromTable(@"%@S", @"Foundation", @"South latitude format, short");
            positiveLongitudeFormat = NSLocalizedStringFromTable(@"%@E", @"Foundation", @"East longitude format, short");
            negativeLongitudeFormat = NSLocalizedStringFromTable(@"%@W", @"Foundation", @"West longitude format, short");
            stringFormat = NSLocalizedStringFromTable(@"%@, %@", @"Foundation", @"Latitude-longitude format, short");
            break;
            
        case NSFormattingUnitStyleMedium:
            positiveLatitudeFormat = NSLocalizedStringFromTable(@"%@ north", @"Foundation", @"North latitude format, medium");
            negativeLatitudeFormat = NSLocalizedStringFromTable(@"%@ south", @"Foundation", @"South latitude format, medium");
            positiveLongitudeFormat = NSLocalizedStringFromTable(@"%@ east", @"Foundation", @"East longitude format, medium");
            negativeLongitudeFormat = NSLocalizedStringFromTable(@"%@ west", @"Foundation", @"West longitude format, medium");
            stringFormat = NSLocalizedStringFromTable(@"%@, %@", @"Foundation", @"Latitude-longitude format, medium");
            break;
            
        case NSFormattingUnitStyleLong:
            positiveLatitudeFormat = NSLocalizedStringFromTable(@"%@ north", @"Foundation", @"North latitude format, long");
            negativeLatitudeFormat = NSLocalizedStringFromTable(@"%@ south", @"Foundation", @"South latitude format, long");
            positiveLongitudeFormat = NSLocalizedStringFromTable(@"%@ east", @"Foundation", @"East longitude format, long");
            negativeLongitudeFormat = NSLocalizedStringFromTable(@"%@ west", @"Foundation", @"West longitude format, long");
            stringFormat = NSLocalizedStringFromTable(@"%@ by %@", @"Foundation", @"Latitude-longitude format, long");
            break;
    }
    NSString *latitudeString = [self stringFromLocationDegrees:coordinate.latitude
                                                positiveFormat:positiveLatitudeFormat
                                                negativeFormat:negativeLatitudeFormat];
    NSString *longitudeString = [self stringFromLocationDegrees:coordinate.longitude
                                                 positiveFormat:positiveLongitudeFormat
                                                 negativeFormat:negativeLongitudeFormat];
    return [NSString stringWithFormat:stringFormat, latitudeString, longitudeString];
}

- (NSString *)stringFromLocationDegrees:(CLLocationDegrees)degrees positiveFormat:(NSString *)positiveFormat negativeFormat:(NSString *)negativeFormat {
    CLLocationDegrees minutes = (fabs(degrees) - floor(fabs(degrees))) * 60;
    CLLocationDegrees seconds = (minutes - floor(minutes)) * 60;
    
    NSString *degreesFormat;
    NSString *minutesFormat;
    NSString *secondsFormat;
    NSString *degreesMinutesFormat;
    NSString *degreesMinutesSecondsFormat;
    switch (self.unitStyle) {
        case NSFormattingUnitStyleShort:
            degreesFormat = NSLocalizedStringFromTable(@"%d°", @"Foundation", @"Degrees format, short");
            minutesFormat = NSLocalizedStringFromTable(@"%d′", @"Foundation", @"Minutes format, short");
            secondsFormat = NSLocalizedStringFromTable(@"%d″", @"Foundation", @"Seconds format, short");
            degreesMinutesFormat = NSLocalizedStringFromTable(@"%@%@", @"Foundation", @"Degrees-minutes format, short");
            degreesMinutesSecondsFormat = NSLocalizedStringFromTable(@"%@%@%@", @"Foundation", @"Degrees-minutes-seconds format, short");
            break;
            
        case NSFormattingUnitStyleMedium:
            degreesFormat = NSLocalizedStringFromTable(@"%d°", @"Foundation", @"Degrees format, medium");
            minutesFormat = NSLocalizedStringFromTable(@"%d′", @"Foundation", @"Minutes format, medium");
            secondsFormat = NSLocalizedStringFromTable(@"%d″", @"Foundation", @"Seconds format, medium");
            degreesMinutesFormat = NSLocalizedStringFromTable(@"%@%@", @"Foundation", @"Degrees-minutes format, medium");
            degreesMinutesSecondsFormat = NSLocalizedStringFromTable(@"%@%@%@", @"Foundation", @"Degrees-minutes-seconds format, medium");
            break;
            
        case NSFormattingUnitStyleLong:
            degreesFormat = NSLocalizedStringFromTable(@"%d degree(s)", @"Foundation", @"Degrees format, long");
            minutesFormat = NSLocalizedStringFromTable(@"%d minute(s)", @"Foundation", @"Minutes format, long");
            secondsFormat = NSLocalizedStringFromTable(@"%d second(s)", @"Foundation", @"Seconds format, long");
            degreesMinutesFormat = NSLocalizedStringFromTable(@"%@ and %@", @"Foundation", @"Degrees-minutes format, long");
            degreesMinutesSecondsFormat = NSLocalizedStringFromTable(@"%@, %@, and %@", @"Foundation", @"Degrees-minutes-seconds format, long");
            break;
    }
    
    NSString *degreesString = [NSString stringWithFormat:degreesFormat, (int)floor(fabs(degrees))];
    
    NSString *string;
    if (trunc(seconds) > 0 && self.allowsSeconds) {
        NSString *minutesString = [NSString stringWithFormat:minutesFormat, (int)floor(minutes)];
        NSString *secondsString = [NSString stringWithFormat:secondsFormat, (int)round(seconds)];
        string = [NSString stringWithFormat:degreesMinutesSecondsFormat,
                  degreesString, minutesString, secondsString];
    } else if (trunc(minutes) > 0 && self.allowsMinutes) {
        NSString *minutesString = [NSString stringWithFormat:minutesFormat, (int)round(minutes)];
        string = [NSString stringWithFormat:degreesMinutesFormat,
                  degreesString, minutesString];
    } else {
        string = [NSString stringWithFormat:degreesFormat, (int)round(fabs(degrees))];
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
