#import "MGLClockDirectionFormatter.h"

#import "NSBundle+MGLAdditions.h"

#define wrap(value, min, max) \
    (fmod((fmod((value - min), (max - min)) + (max - min)), (max - min)) + min)

@implementation MGLClockDirectionFormatter {
    NSNumberFormatter *_numberFormatter;
}

- (instancetype)init {
    if (self = [super init]) {
        _unitStyle = NSFormattingUnitStyleMedium;
        _numberFormatter = [[NSNumberFormatter alloc] init];
    }
    return self;
}

- (NSLocale *)locale {
    return _numberFormatter.locale;
}

- (void)setLocale:(NSLocale *)locale {
    _numberFormatter.locale = locale;
}

- (NSString *)stringFromDirection:(CLLocationDirection)direction {
    NSInteger hour = round(-wrap(-direction, -360, 0) / 360 * 12);
    NSString *format;
    NSNumberFormatterStyle style = NSNumberFormatterDecimalStyle;
    switch (self.unitStyle) {
        case NSFormattingUnitStyleShort:
            format = NSLocalizedString(@"%@:00", @"Clock position format, short style");
            break;
            
        case NSFormattingUnitStyleMedium:
            format = NSLocalizedString(@"%@ o’clock", @"Clock position format, medium style");
            
            break;
            
        case NSFormattingUnitStyleLong:
            format = NSLocalizedString(@"%@ o’clock", @"Clock position format, long style");
            style = NSNumberFormatterSpellOutStyle;
            break;
            
        default:
            break;
    }
    _numberFormatter.numberStyle = style;
    return [NSString stringWithFormat:format, [_numberFormatter stringFromNumber:@(hour)]];
}

- (nullable NSString *)stringForObjectValue:(id)obj {
    if (![obj isKindOfClass:[NSValue class]]) {
        return nil;
    }
    return [self stringFromDirection:[obj doubleValue]];
}

- (BOOL)getObjectValue:(out id __nullable * __nullable)obj forString:(NSString *)string errorDescription:(out NSString * __nullable * __nullable)error {
    NSAssert(NO, @"-getObjectValue:forString:errorDescription: has not been implemented");
    return NO;
}

@end
