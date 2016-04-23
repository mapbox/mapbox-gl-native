#import "MGLCompassDirectionFormatter.h"

#import "NSBundle+MGLAdditions.h"

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
            NSLocalizedStringFromTable(@"N", @"Foundation", @"North, short"),
            NSLocalizedStringFromTable(@"N×E", @"Foundation", @"North by east, short"),
            NSLocalizedStringFromTable(@"NNE", @"Foundation", @"North-northeast, short"),
            NSLocalizedStringFromTable(@"NE×N", @"Foundation", @"Northeast by north, short"),
            NSLocalizedStringFromTable(@"NE", @"Foundation", @"Northeast, short"),
            NSLocalizedStringFromTable(@"NE×E", @"Foundation", @"Northeast by east, short"),
            NSLocalizedStringFromTable(@"ENE", @"Foundation", @"East-northeast, short"),
            NSLocalizedStringFromTable(@"E×N", @"Foundation", @"East by north, short"),
            
            NSLocalizedStringFromTable(@"E", @"Foundation", @"East, short"),
            NSLocalizedStringFromTable(@"E×S", @"Foundation", @"East by south, short"),
            NSLocalizedStringFromTable(@"ESE", @"Foundation", @"East-southeast, short"),
            NSLocalizedStringFromTable(@"SE×E", @"Foundation", @"Southeast by east, short"),
            NSLocalizedStringFromTable(@"SE", @"Foundation", @"Southeast, short"),
            NSLocalizedStringFromTable(@"SE×S", @"Foundation", @"Southeast by south, short"),
            NSLocalizedStringFromTable(@"SSE", @"Foundation", @"South-southeast, short"),
            NSLocalizedStringFromTable(@"S×E", @"Foundation", @"South by east, short"),
            
            NSLocalizedStringFromTable(@"S", @"Foundation", @"South, short"),
            NSLocalizedStringFromTable(@"S×W", @"Foundation", @"South by west, short"),
            NSLocalizedStringFromTable(@"SSW", @"Foundation", @"South-southwest, short"),
            NSLocalizedStringFromTable(@"SW×S", @"Foundation", @"Southwest by south, short"),
            NSLocalizedStringFromTable(@"SW", @"Foundation", @"Southwest, short"),
            NSLocalizedStringFromTable(@"SW×W", @"Foundation", @"Southwest by west, short"),
            NSLocalizedStringFromTable(@"WSW", @"Foundation", @"West-southwest, short"),
            NSLocalizedStringFromTable(@"W×S", @"Foundation", @"West by south, short"),
            
            NSLocalizedStringFromTable(@"W", @"Foundation", @"West, short"),
            NSLocalizedStringFromTable(@"W×N", @"Foundation", @"West by north, short"),
            NSLocalizedStringFromTable(@"WNW", @"Foundation", @"West-northwest, short"),
            NSLocalizedStringFromTable(@"NW×W", @"Foundation", @"Northwest by west, short"),
            NSLocalizedStringFromTable(@"NW", @"Foundation", @"Northwest, short"),
            NSLocalizedStringFromTable(@"NW×N", @"Foundation", @"Northwest by north, short"),
            NSLocalizedStringFromTable(@"NNW", @"Foundation", @"North-northwest, short"),
            NSLocalizedStringFromTable(@"N×W", @"Foundation", @"North by west, short"),
        ];
        
        longStrings = @[
            NSLocalizedStringFromTable(@"north", @"Foundation", @"North, long"),
            NSLocalizedStringFromTable(@"north by east", @"Foundation", @"North by east, long"),
            NSLocalizedStringFromTable(@"north-northeast", @"Foundation", @"North-northeast, long"),
            NSLocalizedStringFromTable(@"northeast by north", @"Foundation", @"Northeast by north, long"),
            NSLocalizedStringFromTable(@"northeast", @"Foundation", @"Northeast, long"),
            NSLocalizedStringFromTable(@"northeast by east", @"Foundation", @"Northeast by east, long"),
            NSLocalizedStringFromTable(@"east-northeast", @"Foundation", @"East-northeast, long"),
            NSLocalizedStringFromTable(@"east by north", @"Foundation", @"East by north, long"),
            
            NSLocalizedStringFromTable(@"east", @"Foundation", @"East, long"),
            NSLocalizedStringFromTable(@"east by south", @"Foundation", @"East by south, long"),
            NSLocalizedStringFromTable(@"east-southeast", @"Foundation", @"East-southeast, long"),
            NSLocalizedStringFromTable(@"southeast by east", @"Foundation", @"Southeast by east, long"),
            NSLocalizedStringFromTable(@"southeast", @"Foundation", @"Southeast, long"),
            NSLocalizedStringFromTable(@"southeast by south", @"Foundation", @"Southeast by south, long"),
            NSLocalizedStringFromTable(@"south-southeast", @"Foundation", @"South-southeast, long"),
            NSLocalizedStringFromTable(@"south by east", @"Foundation", @"South by east, long"),
            
            NSLocalizedStringFromTable(@"south", @"Foundation", @"South, long"),
            NSLocalizedStringFromTable(@"south by west", @"Foundation", @"South by west, long"),
            NSLocalizedStringFromTable(@"south-southwest", @"Foundation", @"South-southwest, long"),
            NSLocalizedStringFromTable(@"southwest by south", @"Foundation", @"Southwest by south, long"),
            NSLocalizedStringFromTable(@"southwest", @"Foundation", @"Southwest, long"),
            NSLocalizedStringFromTable(@"southwest by west", @"Foundation", @"Southwest by west, long"),
            NSLocalizedStringFromTable(@"west-southwest", @"Foundation", @"West-southwest, long"),
            NSLocalizedStringFromTable(@"west by south", @"Foundation", @"West by south, long"),
            
            NSLocalizedStringFromTable(@"west", @"Foundation", @"West, long"),
            NSLocalizedStringFromTable(@"west by north", @"Foundation", @"West by north, long"),
            NSLocalizedStringFromTable(@"west-northwest", @"Foundation", @"West-northwest, long"),
            NSLocalizedStringFromTable(@"northwest by west", @"Foundation", @"Northwest by west, long"),
            NSLocalizedStringFromTable(@"northwest", @"Foundation", @"Northwest, long"),
            NSLocalizedStringFromTable(@"northwest by north", @"Foundation", @"Northwest by north, long"),
            NSLocalizedStringFromTable(@"north-northwest", @"Foundation", @"North-northwest, long"),
            NSLocalizedStringFromTable(@"north by west", @"Foundation", @"North by west, long"),
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
