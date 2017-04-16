#import "MGLDistanceFormatter.h"

@interface MGLDistanceFormatter()
@end

@implementation MGLDistanceFormatter

static const CLLocationDistance METERS_PER_MILE = 1609.344;
static const double YARDS_PER_MILE = 1760.0;
static const double FEET_PER_MILE = YARDS_PER_MILE * 3.0;

- (NSString *)stringFromDistance:(CLLocationDistance)distance {
    double miles = distance / METERS_PER_MILE;
    double feet = miles * FEET_PER_MILE;
    
    NSLengthFormatterUnit unit = NSLengthFormatterUnitMillimeter;
    [self unitStringFromMeters:distance usedUnit:&unit];
    
    self.numberFormatter.roundingIncrement = @0.25;
    
    if (unit == NSLengthFormatterUnitYard) {
        if (miles > 0.2) {
            unit = NSLengthFormatterUnitMile;
            return [self stringFromValue:miles unit:unit];
        } else {
            unit = NSLengthFormatterUnitFoot;
            self.numberFormatter.roundingIncrement = @1;
            return [self stringFromValue:feet unit:unit];
        }
    } else {
        return [self stringFromMeters:distance];
    }
}

@end
