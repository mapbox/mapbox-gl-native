#import "MGLCompassDirectionFormatter.h"

#define wrap(value, min, max) \
    (fmod((fmod((value - min), (max - min)) + (max - min)), (max - min)) + min)

@implementation MGLCompassDirectionFormatter

- (instancetype)init {
    if (self = [super init]) {
        _unitStyle = NSFormattingUnitStyleMedium;
    }
    return self;
}

- (NSString *)stringFromDirection:(CLLocationDirection)direction {
    static NS_ARRAY_OF(NSString *) *shortStrings;
    static NS_ARRAY_OF(NSString *) *longStrings;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        shortStrings = @[
            NSLocalizedString(@"N", @"North, short"),
            NSLocalizedString(@"N×E", @"North by east, short"),
            NSLocalizedString(@"NNE", @"North-northeast, short"),
            NSLocalizedString(@"NE×N", @"Northeast by north, short"),
            NSLocalizedString(@"NE", @"Northeast, short"),
            NSLocalizedString(@"NE×E", @"Northeast by east, short"),
            NSLocalizedString(@"ENE", @"East-northeast, short"),
            NSLocalizedString(@"E×N", @"East by north, short"),
            
            NSLocalizedString(@"E", @"East, short"),
            NSLocalizedString(@"E×S", @"East by south, short"),
            NSLocalizedString(@"ESE", @"East-southeast, short"),
            NSLocalizedString(@"SE×E", @"Southeast by east, short"),
            NSLocalizedString(@"SE", @"Southeast, short"),
            NSLocalizedString(@"SE×S", @"Southeast by south, short"),
            NSLocalizedString(@"SSE", @"South-southeast, short"),
            NSLocalizedString(@"S×E", @"South by east, short"),
            
            NSLocalizedString(@"S", @"South, short"),
            NSLocalizedString(@"S×W", @"South by west, short"),
            NSLocalizedString(@"SSW", @"South-southwest, short"),
            NSLocalizedString(@"SW×S", @"Southwest by south, short"),
            NSLocalizedString(@"SW", @"Southwest, short"),
            NSLocalizedString(@"SW×W", @"Southwest by west, short"),
            NSLocalizedString(@"WSW", @"West-southwest, short"),
            NSLocalizedString(@"W×S", @"West by south, short"),
            
            NSLocalizedString(@"W", @"West, short"),
            NSLocalizedString(@"W×N", @"West by north, short"),
            NSLocalizedString(@"WNW", @"West-northwest, short"),
            NSLocalizedString(@"NW×W", @"Northwest by west, short"),
            NSLocalizedString(@"NW", @"Northwest, short"),
            NSLocalizedString(@"NW×N", @"Northwest by north, short"),
            NSLocalizedString(@"NNW", @"North-northwest, short"),
            NSLocalizedString(@"N×W", @"North by west, short"),
        ];
        
        longStrings = @[
            NSLocalizedString(@"north", @"North, long"),
            NSLocalizedString(@"north by east", @"North by east, long"),
            NSLocalizedString(@"north-northeast", @"North-northeast, long"),
            NSLocalizedString(@"northeast by north", @"Northeast by north, long"),
            NSLocalizedString(@"northeast", @"Northeast, long"),
            NSLocalizedString(@"northeast by east", @"Northeast by east, long"),
            NSLocalizedString(@"east-northeast", @"East-northeast, long"),
            NSLocalizedString(@"east by north", @"East by north, long"),
            
            NSLocalizedString(@"east", @"East, long"),
            NSLocalizedString(@"east by south", @"East by south, long"),
            NSLocalizedString(@"east-southeast", @"East-southeast, long"),
            NSLocalizedString(@"southeast by east", @"Southeast by east, long"),
            NSLocalizedString(@"southeast", @"Southeast, long"),
            NSLocalizedString(@"southeast by south", @"Southeast by south, long"),
            NSLocalizedString(@"south-southeast", @"South-southeast, long"),
            NSLocalizedString(@"south by east", @"South by east, long"),
            
            NSLocalizedString(@"south", @"South, long"),
            NSLocalizedString(@"south by west", @"South by west, long"),
            NSLocalizedString(@"south-southwest", @"South-southwest, long"),
            NSLocalizedString(@"southwest by south", @"Southwest by south, long"),
            NSLocalizedString(@"southwest", @"Southwest, long"),
            NSLocalizedString(@"southwest by west", @"Southwest by west, long"),
            NSLocalizedString(@"west-southwest", @"West-southwest, long"),
            NSLocalizedString(@"west by south", @"West by south, long"),
            
            NSLocalizedString(@"west", @"West, long"),
            NSLocalizedString(@"west by north", @"West by north, long"),
            NSLocalizedString(@"west-northwest", @"West-northwest, long"),
            NSLocalizedString(@"northwest by west", @"Northwest by west, long"),
            NSLocalizedString(@"northwest", @"Northwest, long"),
            NSLocalizedString(@"northwest by north", @"Northwest by north, long"),
            NSLocalizedString(@"north-northwest", @"North-northwest, long"),
            NSLocalizedString(@"north by west", @"North by west, long"),
        ];
        
        NSAssert(shortStrings.count == longStrings.count, @"Long and short compass direction string arrays must have the same size.");
    });
    
    NSInteger cardinalPoint = round(wrap(direction, 0, 360) / 360 * shortStrings.count);
    switch (self.unitStyle) {
        case NSFormattingUnitStyleShort:
            return shortStrings[cardinalPoint];
            
        case NSFormattingUnitStyleMedium:
        case NSFormattingUnitStyleLong:
            return longStrings[cardinalPoint];
    }
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
