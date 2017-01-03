#import "MGLDistanceFormatter.h"

#import "NSBundle+MGLAdditions.h"


@interface MGLDistanceFormatter()
@property (nonatomic) NSNumberFormatter *numberFormatter;
@end

@implementation MGLDistanceFormatter

static const CLLocationDistance METERS_PER_KM = 1000;
static const CLLocationDistance METERS_PER_MILE = 1609.34;
static const CLLocationDistance METERS_PER_YARD = 0.9144;
static const CLLocationDistance METERS_PER_FOOT = 0.3048;

typedef NS_ENUM(NSUInteger, MGLDistanceFormatterUnit) {
    MGLDistanceFormatterUnitMeter,
    MGLDistanceFormatterUnitKilometer,
    MGLDistanceFormatterUnitFoot,
    MGLDistanceFormatterUnitMile,
    MGLDistanceFormatterUnitYard
};

- (instancetype)init {
    if (self = [super init]) {
        _unitStyle = MGLDistanceFormatterUnitStyleDefault;
        
        _numberFormatter = [[NSNumberFormatter alloc] init];
        _numberFormatter.minimumFractionDigits = 0;
        _numberFormatter.roundingMode = NSNumberFormatterRoundHalfUp;
    }
    return self;
}

- (NSString *)stringFromDistance:(CLLocationDistance)distance {
    if (distance < 0)
        return nil;
    
    NSString *format = [self formatForDistance:distance];
    NSNumber *convertedDistance = @([self convertedDistance:distance]);
    
    MGLDistanceFormatterUnit unit = [self unitForDistance:distance];
    
    switch (self.preferredUnits) {
        case MGLDistanceFormatterUnitsDefault:
        case MGLDistanceFormatterUnitsMetric:
            _numberFormatter.maximumFractionDigits = unit == MGLDistanceFormatterUnitMeter ? 0 : 1;
            return [NSString stringWithFormat:format, [self.numberFormatter numberFromString:[self.numberFormatter stringFromNumber:convertedDistance]]];
        case MGLDistanceFormatterUnitsImperial:
            _numberFormatter.maximumFractionDigits = unit == MGLDistanceFormatterUnitFoot ? 0 : 1;
            return [NSString stringWithFormat:format, [self.numberFormatter numberFromString:[self.numberFormatter stringFromNumber:convertedDistance]]];
        case MGLDistanceFormatterUnitsImperialWithYards:
            _numberFormatter.maximumFractionDigits = unit == MGLDistanceFormatterUnitYard ? 0 : 1;
            return [NSString stringWithFormat:format, [self.numberFormatter numberFromString:[self.numberFormatter stringFromNumber:convertedDistance]]];
    }
}

- (CLLocationDistance)convertedDistance:(CLLocationDistance)distance {
    MGLDistanceFormatterUnit unit = [self unitForDistance:distance];
    switch (unit) {
        case MGLDistanceFormatterUnitMeter:
            return distance;
        case MGLDistanceFormatterUnitKilometer:
            return distance / METERS_PER_KM;
        case MGLDistanceFormatterUnitFoot:
            return distance / METERS_PER_FOOT;
        case MGLDistanceFormatterUnitMile:
            return distance / METERS_PER_MILE;
        case MGLDistanceFormatterUnitYard:
            return distance / METERS_PER_YARD;
    }
}

- (NSString *)formatForDistance:(CLLocationDistance)distance {
    MGLDistanceFormatterUnit unit = [self unitForDistance:distance];
    
    switch (self.unitStyle) {
        case MGLDistanceFormatterUnitStyleDefault:
        case MGLDistanceFormatterUnitStyleAbbreviated:
            switch (unit) {
                case MGLDistanceFormatterUnitMeter:
                    return NSLocalizedStringWithDefaultValue(@"DISTANCE_METER_SHORT", @"Foundation", nil, @"%@ m", @"Distance format, short: {1 m}");
                case MGLDistanceFormatterUnitKilometer:
                    return NSLocalizedStringWithDefaultValue(@"DISTANCE_KILOMETER_SHORT", @"Foundation", nil, @"%@ km", @"Distance format, short: {1 km}");
                case MGLDistanceFormatterUnitFoot:
                    return NSLocalizedStringWithDefaultValue(@"DISTANCE_FOOT_SHORT", @"Foundation", nil, @"%@ ft", @"Distance format, short: {1 ft}");
                case MGLDistanceFormatterUnitMile:
                    return NSLocalizedStringWithDefaultValue(@"DISTANCE_MILE_SHORT", @"Foundation", nil, @"%@ mi", @"Distance format, short: {1 mi}");
                case MGLDistanceFormatterUnitYard:
                    return NSLocalizedStringWithDefaultValue(@"DISTANCE_YARD_SHORT", @"Foundation", nil, @"%@ yd", @"Distance format, short: {1 yd}");
            }
        case MGLDistanceFormatterUnitStyleFull:
            switch (unit) {
                case MGLDistanceFormatterUnitMeter:
                    return NSLocalizedStringWithDefaultValue(@"DISTANCE_METER_LONG", @"Foundation", nil, @"%@ meter(s)", @"Distance format, long: {1 meter}");
                case MGLDistanceFormatterUnitKilometer:
                    return NSLocalizedStringWithDefaultValue(@"DISTANCE_KILOMETER_LONG", @"Foundation", nil, @"%@ kilometer(s)", @"Distance format, long: {1 kilometer}");
                case MGLDistanceFormatterUnitFoot:
                    return NSLocalizedStringWithDefaultValue(@"DISTANCE_FOOT_LONG", @"Foundation", nil, @"%@ foot/feet", @"Distance format, long: {1 foot}");
                case MGLDistanceFormatterUnitMile:
                    return NSLocalizedStringWithDefaultValue(@"DISTANCE_MILE_LONG", @"Foundation", nil, @"%@ mile(s)", @"Distance format, long: {1 mile}");
                case MGLDistanceFormatterUnitYard:
                    return NSLocalizedStringWithDefaultValue(@"DISTANCE_YARD_LONG", @"Foundation", nil, @"%@ yard(s)", @"Distance format, long: {1 yard}");
            }
    }
}

- (MGLDistanceFormatterUnit)unitForDistance:(CLLocationDistance)distance {
    switch (self.preferredUnits) {
        // MGLDistanceFormatterUnitsDefault is only added to make the switch statement
        // exhaustive, it is never returned from -[MGLDistanceFormatter preferredUnits]
        case MGLDistanceFormatterUnitsDefault:
        case MGLDistanceFormatterUnitsMetric:
            return distance < METERS_PER_KM ? MGLDistanceFormatterUnitMeter : MGLDistanceFormatterUnitKilometer;
        case MGLDistanceFormatterUnitsImperial: {
            double ft = distance / METERS_PER_FOOT;
            return ft < 1000 ? MGLDistanceFormatterUnitFoot : MGLDistanceFormatterUnitMile;
        }
        case MGLDistanceFormatterUnitsImperialWithYards: {
            double yd = distance / METERS_PER_YARD;
            return yd < 1000 ? MGLDistanceFormatterUnitYard : MGLDistanceFormatterUnitMile;
        }
    }
}

- (MGLDistanceFormatterUnits)preferredUnits {
    if (self.units == MGLDistanceFormatterUnitsDefault) {
        BOOL usesMetric = [[[NSLocale currentLocale] objectForKey:NSLocaleUsesMetricSystem] boolValue];
        return usesMetric ? MGLDistanceFormatterUnitsMetric : MGLDistanceFormatterUnitsImperial;
    }
    return self.units;
}

- (BOOL)getObjectValue:(out id __nullable * __nullable)obj forString:(NSString *)string errorDescription:(out NSString * __nullable * __nullable)error {
    return NO;
}

@end
