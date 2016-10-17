#import "MGLStyleAttributeFunction.h"

#import "MGLStyleLayer_Private.h"
#import "MGLStyleAttributeValue_Private.h"
#import "MGLStyleAttributeFunction_Private.h"

@interface MGLStyleAttributeFunction() <MGLStyleAttributeValue_Private>
@end

@implementation MGLStyleAttributeFunction

- (instancetype)init
{
    if (self = [super init]) {
        _base = @1;
    }
    return self;
}

- (BOOL)isFunction
{
    return YES;
}

- (mbgl::style::PropertyValue<mbgl::Color>)mgl_colorPropertyValue
{
    __block std::vector<std::pair<float, mbgl::Color>> stops;
    [self.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, MGLColor * _Nonnull color, BOOL * _Nonnull stop) {
        NSAssert([color isKindOfClass:[MGLColor class]], @"Stops should be colors");
        stops.emplace_back(zoomKey.floatValue, color.mgl_color);
    }];
    return mbgl::style::Function<mbgl::Color>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<float>)mgl_floatPropertyValue
{
    __block std::vector<std::pair<float, float>> stops;
    [self.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSNumber * _Nonnull number, BOOL * _Nonnull stop) {
        NSAssert([number isKindOfClass:[NSNumber class]], @"Stops should be NSNumbers");
        stops.emplace_back(zoomKey.floatValue, number.floatValue);
    }];
    return mbgl::style::Function<float>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<bool>)mgl_boolPropertyValue
{
    __block std::vector<std::pair<float, bool>> stops;
    [self.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSNumber * _Nonnull number, BOOL * _Nonnull stop) {
        NSAssert([number isKindOfClass:[NSNumber class]], @"Stops should be NSNumbers");
        stops.emplace_back(zoomKey.floatValue, number.boolValue);
    }];
    return mbgl::style::Function<bool>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<std::string>)mgl_stringPropertyValue
{
    __block std::vector<std::pair<float, std::string>> stops;
    [self.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSString * _Nonnull string, BOOL * _Nonnull stop) {
        NSAssert([string isKindOfClass:[NSString class]], @"Stops should be strings");
        stops.emplace_back(zoomKey.floatValue, string.UTF8String);
    }];
    return mbgl::style::Function<std::string>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<std::vector<std::string> >)mgl_stringArrayPropertyValue
{
    __block std::vector<std::pair<float, std::vector<std::string>>> stops;
    [self.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSArray *  _Nonnull strings, BOOL * _Nonnull stop) {
        NSAssert([strings isKindOfClass:[NSArray class]], @"Stops should be NSArray");
        std::vector<std::string>convertedStrings;
        for (NSString *string in strings) {
            convertedStrings.emplace_back(string.UTF8String);
        }
        stops.emplace_back(zoomKey.floatValue, convertedStrings);
    }];
    return mbgl::style::Function<std::vector<std::string>>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<std::vector<float> >)mgl_numberArrayPropertyValue
{
    __block std::vector<std::pair<float, std::vector<float>>> stops;
    [self.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSArray *  _Nonnull numbers, BOOL * _Nonnull stop) {
        NSAssert([numbers isKindOfClass:[NSArray class]], @"Stops should be NSArray");
        std::vector<float>convertedNumbers;
        for (NSNumber *number in numbers) {
            convertedNumbers.emplace_back(number.floatValue);
        }
        stops.emplace_back(zoomKey.floatValue, convertedNumbers);
    }];
    return mbgl::style::Function<std::vector<float>>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<std::array<float, 4> >)mgl_paddingPropertyValue
{
    __block std::vector<std::pair<float, std::array<float, 4>>> stops;
    [self.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSValue * _Nonnull padding, BOOL * _Nonnull stop) {
        NSAssert([padding isKindOfClass:[NSValue class]], @"Stops should be NSValue");
        stops.emplace_back(zoomKey.floatValue, padding.mgl_paddingArrayValue);
    }];
    return mbgl::style::Function<std::array<float, 4>>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<std::array<float, 2> >)mgl_offsetPropertyValue
{
    __block std::vector<std::pair<float, std::array<float, 2>>> stops;
    [self.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSValue * _Nonnull offset, BOOL * _Nonnull stop) {
        NSAssert([offset isKindOfClass:[NSValue class]], @"Stops should be NSValue");
        stops.emplace_back(zoomKey.floatValue, offset.mgl_offsetArrayValue);
    }];
    return mbgl::style::Function<std::array<float, 2>>({{stops}}, _base.floatValue);
}

+ (instancetype)functionWithColorPropertyValue:(mbgl::style::Function<mbgl::Color>)property
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    auto stops = property.getStops();
    NSMutableDictionary *convertedStops = [NSMutableDictionary dictionaryWithCapacity:stops.size()];
    for (auto stop : stops) {
        convertedStops[@(stop.first)] = [MGLColor mgl_colorWithColor:stop.second];
    }
    function.base = @(property.getBase());
    function.stops = convertedStops;
    return function;
}

+ (instancetype)functionWithNumberPropertyValue:(mbgl::style::Function<float>)property
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    auto stops = property.getStops();
    NSMutableDictionary *convertedStops = [NSMutableDictionary dictionaryWithCapacity:stops.size()];
    for (auto stop : stops) {
        convertedStops[@(stop.first)] = @(stop.second);
    }
    function.base = @(property.getBase());
    function.stops = convertedStops;
    return function;
}

+ (instancetype)functionWithBoolPropertyValue:(mbgl::style::Function<bool>)property
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    auto stops = property.getStops();
    NSMutableDictionary *convertedStops = [NSMutableDictionary dictionaryWithCapacity:stops.size()];
    for (auto stop : stops) {
        convertedStops[@(stop.first)] = @(stop.second);
    }
    function.base = @(property.getBase());
    function.stops = convertedStops;
    return function;
}

+ (instancetype)functionWithStringPropertyValue:(mbgl::style::Function<std::string>)property
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    auto stops = property.getStops();
    NSMutableDictionary *convertedStops = [NSMutableDictionary dictionaryWithCapacity:stops.size()];
    for (auto stop : stops) {
        convertedStops[@(stop.first)] = @(stop.second.c_str());
    }
    function.base = @(property.getBase());
    function.stops = convertedStops;
    return function;
}

+ (instancetype)functionWithOffsetPropertyValue:(mbgl::style::Function<std::array<float, 2> >)property
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    auto stops = property.getStops();
    NSMutableDictionary *convertedStops = [NSMutableDictionary dictionaryWithCapacity:stops.size()];
    for (auto stop : stops) {
        convertedStops[@(stop.first)] = [NSValue mgl_valueWithOffsetArray:stop.second];
    }
    function.base = @(property.getBase());
    function.stops = convertedStops;
    return function;
}

+ (instancetype)functionWithPaddingPropertyValue:(mbgl::style::Function<std::array<float, 4> >)property
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    auto stops = property.getStops();
    NSMutableDictionary *convertedStops = [NSMutableDictionary dictionaryWithCapacity:stops.size()];
    for (auto stop : stops) {
        convertedStops[@(stop.first)] = [NSValue mgl_valueWithPaddingArray:stop.second];
    }
    function.base = @(property.getBase());
    function.stops = convertedStops;
    return function;
}

+ (instancetype)functionWithStringArrayPropertyValue:(mbgl::style::Function<std::vector<std::string> >)property
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    auto stops = property.getStops();
    NSMutableDictionary *convertedStops = [NSMutableDictionary dictionaryWithCapacity:stops.size()];
    for (auto stop : stops) {
        auto strings = stop.second;
        NSMutableArray *convertedStrings = [NSMutableArray arrayWithCapacity:strings.size()];
        for (auto const& string: strings) {
            [convertedStrings addObject:@(string.c_str())];
        }
        convertedStops[@(stop.first)] = convertedStrings;
    }
    function.base = @(property.getBase());
    function.stops = convertedStops;
    return function;
}

+ (instancetype)functionWithNumberArrayPropertyValue:(mbgl::style::Function<std::vector<float> >)property
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    auto stops = property.getStops();
    NSMutableDictionary *convertedStops = [NSMutableDictionary dictionaryWithCapacity:stops.size()];
    for (auto stop : stops) {
        auto numbers = stop.second;
        NSMutableArray *convertedNumbers = [NSMutableArray arrayWithCapacity:numbers.size()];
        for (auto const& number: numbers) {
            [convertedNumbers addObject:@(number)];
        }
        convertedStops[@(stop.first)] = convertedNumbers;
    }
    function.base = @(property.getBase());
    function.stops = convertedStops;
    return function;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p, base = %@; stops = %@>",
            NSStringFromClass([self class]), (void *)self,
            self.base,
            self.stops];
}

- (BOOL)isEqual:(MGLStyleAttributeFunction *)other
{
    return ([other isKindOfClass:[self class]]
            && (other.base == self.base || [other.base isEqualToNumber:self.base])
            && [other.stops isEqualToDictionary:self.stops]);
}

- (NSUInteger)hash
{
    return self.base.hash + self.stops.hash;
}

@end
