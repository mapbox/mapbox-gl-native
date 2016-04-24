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
            NSLocalizedStringWithDefaultValue(@"COMPASS_N_SHORT", @"Foundation", nil, @"N", @"North, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NxE_SHORT", @"Foundation", nil, @"N×E", @"North by east, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NNE_SHORT", @"Foundation", nil, @"NNE", @"North-northeast, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NExN_SHORT", @"Foundation", nil, @"NE×N", @"Northeast by north, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NE_SHORT", @"Foundation", nil, @"NE", @"Northeast, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NExE_SHORT", @"Foundation", nil, @"NE×E", @"Northeast by east, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_ENE_SHORT", @"Foundation", nil, @"ENE", @"East-northeast, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_ExN_SHORT", @"Foundation", nil, @"E×N", @"East by north, short"),
            
            NSLocalizedStringWithDefaultValue(@"COMPASS_E_SHORT", @"Foundation", nil, @"E", @"East, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_ExS_SHORT", @"Foundation", nil, @"E×S", @"East by south, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_ESE_SHORT", @"Foundation", nil, @"ESE", @"East-southeast, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SExE_SHORT", @"Foundation", nil, @"SE×E", @"Southeast by east, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SE_SHORT", @"Foundation", nil, @"SE", @"Southeast, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SExS_SHORT", @"Foundation", nil, @"SE×S", @"Southeast by south, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SSE_SHORT", @"Foundation", nil, @"SSE", @"South-southeast, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SxE_SHORT", @"Foundation", nil, @"S×E", @"South by east, short"),
            
            NSLocalizedStringWithDefaultValue(@"COMPASS_S_SHORT", @"Foundation", nil, @"S", @"South, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SxW_SHORT", @"Foundation", nil, @"S×W", @"South by west, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SSW_SHORT", @"Foundation", nil, @"SSW", @"South-southwest, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SWxS_SHORT", @"Foundation", nil, @"SW×S", @"Southwest by south, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SW_SHORT", @"Foundation", nil, @"SW", @"Southwest, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SWxW_SHORT", @"Foundation", nil, @"SW×W", @"Southwest by west, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_WSW_SHORT", @"Foundation", nil, @"WSW", @"West-southwest, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_WxS_SHORT", @"Foundation", nil, @"W×S", @"West by south, short"),
            
            NSLocalizedStringWithDefaultValue(@"COMPASS_W_SHORT", @"Foundation", nil, @"W", @"West, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_WxN_SHORT", @"Foundation", nil, @"W×N", @"West by north, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_WNW_SHORT", @"Foundation", nil, @"WNW", @"West-northwest, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NWxW_SHORT", @"Foundation", nil, @"NW×W", @"Northwest by west, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NW_SHORT", @"Foundation", nil, @"NW", @"Northwest, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NWxN_SHORT", @"Foundation", nil, @"NW×N", @"Northwest by north, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NNW_SHORT", @"Foundation", nil, @"NNW", @"North-northwest, short"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NxW_SHORT", @"Foundation", nil, @"N×W", @"North by west, short"),
        ];
        
        longStrings = @[
            NSLocalizedStringWithDefaultValue(@"COMPASS_N_LONG", @"Foundation", nil, @"north", @"North, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NxE_LONG", @"Foundation", nil, @"north by east", @"North by east, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NNE_LONG", @"Foundation", nil, @"north-northeast", @"North-northeast, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NExN_LONG", @"Foundation", nil, @"northeast by north", @"Northeast by north, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NE_LONG", @"Foundation", nil, @"northeast", @"Northeast, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NExE_LONG", @"Foundation", nil, @"northeast by east", @"Northeast by east, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_ENE_LONG", @"Foundation", nil, @"east-northeast", @"East-northeast, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_ExN_LONG", @"Foundation", nil, @"east by north", @"East by north, long"),
            
            NSLocalizedStringWithDefaultValue(@"COMPASS_E_LONG", @"Foundation", nil, @"east", @"East, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_ExS_LONG", @"Foundation", nil, @"east by south", @"East by south, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_ESE_LONG", @"Foundation", nil, @"east-southeast", @"East-southeast, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SExE_LONG", @"Foundation", nil, @"southeast by east", @"Southeast by east, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SE_LONG", @"Foundation", nil, @"southeast", @"Southeast, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SExS_LONG", @"Foundation", nil, @"southeast by south", @"Southeast by south, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SSE_LONG", @"Foundation", nil, @"south-southeast", @"South-southeast, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SxE_LONG", @"Foundation", nil, @"south by east", @"South by east, long"),
            
            NSLocalizedStringWithDefaultValue(@"COMPASS_S_LONG", @"Foundation", nil, @"south", @"South, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SxW_LONG", @"Foundation", nil, @"south by west", @"South by west, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SSW_LONG", @"Foundation", nil, @"south-southwest", @"South-southwest, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SWxS_LONG", @"Foundation", nil, @"southwest by south", @"Southwest by south, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SW_LONG", @"Foundation", nil, @"southwest", @"Southwest, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_SWxW_LONG", @"Foundation", nil, @"southwest by west", @"Southwest by west, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_WSW_LONG", @"Foundation", nil, @"west-southwest", @"West-southwest, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_WxS_LONG", @"Foundation", nil, @"west by south", @"West by south, long"),
            
            NSLocalizedStringWithDefaultValue(@"COMPASS_W_LONG", @"Foundation", nil, @"west", @"West, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_WxN_LONG", @"Foundation", nil, @"west by north", @"West by north, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_WNW_LONG", @"Foundation", nil, @"west-northwest", @"West-northwest, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NWxW_LONG", @"Foundation", nil, @"northwest by west", @"Northwest by west, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NW_LONG", @"Foundation", nil, @"northwest", @"Northwest, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NWxN_LONG", @"Foundation", nil, @"northwest by north", @"Northwest by north, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NNW_LONG", @"Foundation", nil, @"north-northwest", @"North-northwest, long"),
            NSLocalizedStringWithDefaultValue(@"COMPASS_NxW_LONG", @"Foundation", nil, @"north by west", @"North by west, long"),
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
