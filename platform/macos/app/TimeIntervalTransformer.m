#import "TimeIntervalTransformer.h"

@implementation TimeIntervalTransformer

+ (Class)transformedValueClass {
    return [NSString class];
}

+ (BOOL)allowsReverseTransformation {
    return NO;
}

NSString *NumberAndUnitString(NSInteger quantity, NSString *singular, NSString *plural) {
    return [NSString stringWithFormat:@"%ld %@", quantity, quantity == 1 ? singular : plural];
}

- (id)transformedValue:(id)value {
    if (![value isKindOfClass:[NSValue class]]) {
        return nil;
    }

    NSTimeInterval timeInterval = [value doubleValue];
    NSInteger seconds = floor(timeInterval);
    NSInteger minutes = floor(seconds / 60);
    seconds -= minutes * 60;
    NSInteger hours = floor(minutes / 60);
    minutes -= hours * 60;
    NSInteger days = floor(hours / 24);
    hours -= days * 24;
    NSInteger weeks = floor(days) / 7;
    days -= weeks * 7;

    NSMutableArray *components = [NSMutableArray array];
    if (seconds || timeInterval < 60) {
        [components addObject:NumberAndUnitString(seconds, @"second", @"seconds")];
    }
    if (minutes) {
        [components insertObject:NumberAndUnitString(minutes, @"minute", @"minutes") atIndex:0];
    }
    if (hours) {
        [components insertObject:NumberAndUnitString(hours, @"hour", @"hours") atIndex:0];
    }
    if (days) {
        [components insertObject:NumberAndUnitString(days, @"day", @"days") atIndex:0];
    }
    if (weeks) {
        [components insertObject:NumberAndUnitString(weeks, @"week", @"weeks") atIndex:0];
    }

    return [components componentsJoinedByString:@", "];
}

@end
