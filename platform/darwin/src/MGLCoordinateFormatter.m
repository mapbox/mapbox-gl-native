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
            positiveLatitudeFormat = NSLocalizedStringWithDefaultValue(@"COORD_N_SHORT", @"Foundation", nil, @"%@N", @"North latitude format, short: {latitude}");
            negativeLatitudeFormat = NSLocalizedStringWithDefaultValue(@"COORD_S_SHORT", @"Foundation", nil, @"%@S", @"South latitude format, short: {latitude}");
            positiveLongitudeFormat = NSLocalizedStringWithDefaultValue(@"COORD_E_SHORT", @"Foundation", nil, @"%@E", @"East longitude format, short: {longitude}");
            negativeLongitudeFormat = NSLocalizedStringWithDefaultValue(@"COORD_W_SHORT", @"Foundation", nil, @"%@W", @"West longitude format, short: {longitude}");
            stringFormat = NSLocalizedStringWithDefaultValue(@"COORD_FMT_SHORT", @"Foundation", nil, @"%@, %@", @"Coordinate pair format, short: {latitude}, {longitude}");
            break;

        case NSFormattingUnitStyleMedium:
            positiveLatitudeFormat = NSLocalizedStringWithDefaultValue(@"COORD_N_MEDIUM", @"Foundation", nil, @"%@ north", @"North latitude format, medium: {latitude}");
            negativeLatitudeFormat = NSLocalizedStringWithDefaultValue(@"COORD_S_MEDIUM", @"Foundation", nil, @"%@ south", @"South latitude format, medium: {latitude}");
            positiveLongitudeFormat = NSLocalizedStringWithDefaultValue(@"COORD_E_MEDIUM", @"Foundation", nil, @"%@ east", @"East longitude format, medium: {longitude}");
            negativeLongitudeFormat = NSLocalizedStringWithDefaultValue(@"COORD_W_MEDIUM", @"Foundation", nil, @"%@ west", @"West longitude format, medium: {longitude}");
            stringFormat = NSLocalizedStringWithDefaultValue(@"COORD_FMT_MEDIUM", @"Foundation", nil, @"%@, %@", @"Coordinate pair format, medium: {latitude}, {longitude}");
            break;

        case NSFormattingUnitStyleLong:
            positiveLatitudeFormat = NSLocalizedStringWithDefaultValue(@"COORD_N_LONG", @"Foundation", nil, @"%@ north", @"North latitude format, long: {latitude}");
            negativeLatitudeFormat = NSLocalizedStringWithDefaultValue(@"COORD_S_LONG", @"Foundation", nil, @"%@ south", @"South latitude format, long: {latitude}");
            positiveLongitudeFormat = NSLocalizedStringWithDefaultValue(@"COORD_E_LONG", @"Foundation", nil, @"%@ east", @"East longitude format, long: {longitude}");
            negativeLongitudeFormat = NSLocalizedStringWithDefaultValue(@"COORD_W_LONG", @"Foundation", nil, @"%@ west", @"West longitude format, long: {longitude}");
            stringFormat = NSLocalizedStringWithDefaultValue(@"COORD_FMT_LONG", @"Foundation", nil, @"%@ by %@", @"Coordinate pair format, long: {latitude}, {longitude}");
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
            degreesFormat = NSLocalizedStringWithDefaultValue(@"COORD_DEG_SHORT", @"Foundation", nil, @"%d°", @"Degrees format, short: {degrees}");
            minutesFormat = NSLocalizedStringWithDefaultValue(@"COORD_MIN_SHORT", @"Foundation", nil, @"%d′", @"Minutes format, short: {minutes}");
            secondsFormat = NSLocalizedStringWithDefaultValue(@"COORD_SEC_SHORT", @"Foundation", nil, @"%d″", @"Seconds format, short: {seconds}");
            degreesMinutesFormat = NSLocalizedStringWithDefaultValue(@"COORD_DM_SHORT", @"Foundation", nil, @"%@%@", @"Coordinate format, short: {degrees}{minutes}");
            degreesMinutesSecondsFormat = NSLocalizedStringWithDefaultValue(@"COORD_DMS_SHORT", @"Foundation", nil, @"%@%@%@", @"Coordinate format, short: {degrees}{minutes}{seconds}");
            break;

        case NSFormattingUnitStyleMedium:
            degreesFormat = NSLocalizedStringWithDefaultValue(@"COORD_DEG_MEDIUM", @"Foundation", nil, @"%d°", @"Degrees format, medium: {degrees}");
            minutesFormat = NSLocalizedStringWithDefaultValue(@"COORD_MIN_MEDIUM", @"Foundation", nil, @"%d′", @"Minutes format, medium: {minutes}");
            secondsFormat = NSLocalizedStringWithDefaultValue(@"COORD_SEC_MEDIUM", @"Foundation", nil, @"%d″", @"Seconds format, medium: {seconds}");
            degreesMinutesFormat = NSLocalizedStringWithDefaultValue(@"COORD_DM_MEDIUM", @"Foundation", nil, @"%@%@", @"Coordinate format, medium: {degrees}{minutes}");
            degreesMinutesSecondsFormat = NSLocalizedStringWithDefaultValue(@"COORD_DMS_MEDIUM", @"Foundation", nil, @"%@%@%@", @"Coordinate format, medium: {degrees}{minutes}{seconds}");
            break;

        case NSFormattingUnitStyleLong:
            degreesFormat = NSLocalizedStringWithDefaultValue(@"COORD_DEG_LONG", @"Foundation", nil, @"%d degree(s)", @"Degrees format, long");
            minutesFormat = NSLocalizedStringWithDefaultValue(@"COORD_MIN_LONG", @"Foundation", nil, @"%d minute(s)", @"Minutes format, long");
            secondsFormat = NSLocalizedStringWithDefaultValue(@"COORD_SEC_LONG", @"Foundation", nil, @"%d second(s)", @"Seconds format, long");
            degreesMinutesFormat = NSLocalizedStringWithDefaultValue(@"COORD_DM_LONG", @"Foundation", nil, @"%@ and %@", @"Coordinate format, long: {degrees}{minutes}");
            degreesMinutesSecondsFormat = NSLocalizedStringWithDefaultValue(@"COORD_DMS_LONG", @"Foundation", nil, @"%@, %@, and %@", @"Coordinate format, long: {degrees}{minutes}{seconds}");
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
