#import "MGLStyleAttributeFunction.h"

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue_Private.hpp"
#import "MGLStyleAttributeFunction_Private.hpp"

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

- (mbgl::style::PropertyValue<mbgl::Color>)mbgl_colorPropertyValue
{
    __block std::vector<std::pair<float, mbgl::Color>> stops;
    [self.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, MGLColor * _Nonnull color, BOOL * _Nonnull stop) {
        NSAssert([color isKindOfClass:[MGLColor class]], @"Stops should be colors");
        stops.emplace_back(zoomKey.floatValue, color.mbgl_color);
    }];
    return mbgl::style::Function<mbgl::Color>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<float>)mbgl_floatPropertyValue
{
    __block std::vector<std::pair<float, float>> stops;
    [self.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSNumber * _Nonnull number, BOOL * _Nonnull stop) {
        NSAssert([number isKindOfClass:[NSNumber class]], @"Stops should be NSNumbers");
        stops.emplace_back(zoomKey.floatValue, number.floatValue);
    }];
    return mbgl::style::Function<float>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<bool>)mbgl_boolPropertyValue
{
    __block std::vector<std::pair<float, bool>> stops;
    [self.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSNumber * _Nonnull number, BOOL * _Nonnull stop) {
        NSAssert([number isKindOfClass:[NSNumber class]], @"Stops should be NSNumbers");
        stops.emplace_back(zoomKey.floatValue, number.boolValue);
    }];
    return mbgl::style::Function<bool>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<std::string>)mbgl_stringPropertyValue
{
    __block std::vector<std::pair<float, std::string>> stops;
    [self.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSString * _Nonnull string, BOOL * _Nonnull stop) {
        NSAssert([string isKindOfClass:[NSString class]], @"Stops should be strings");
        stops.emplace_back(zoomKey.floatValue, string.UTF8String);
    }];
    return mbgl::style::Function<std::string>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<std::vector<std::string> >)mbgl_stringArrayPropertyValue
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

- (mbgl::style::PropertyValue<std::vector<float> >)mbgl_numberArrayPropertyValue
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

- (mbgl::style::PropertyValue<std::array<float, 4> >)mbgl_paddingPropertyValue
{
    __block std::vector<std::pair<float, std::array<float, 4>>> stops;
    [self.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSArray *  _Nonnull padding, BOOL * _Nonnull stop) {
        NSAssert([padding isKindOfClass:[NSArray class]], @"Stops should be NSArray");
        NSNumber *top = padding[0];
        NSNumber *left = padding[1];
        NSNumber *bottom = padding[2];
        NSNumber *right = padding[2];
        auto pad = std::array<float, 4>({{top.floatValue, left.floatValue, bottom.floatValue, right.floatValue}});
        stops.emplace_back(zoomKey.floatValue, pad);
    }];
    return mbgl::style::Function<std::array<float, 4>>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<std::array<float, 2> >)mbgl_offsetPropertyValue
{
    __block std::vector<std::pair<float, std::array<float, 2>>> stops;
    [self.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSArray *  _Nonnull offset, BOOL * _Nonnull stop) {
        NSAssert([offset isKindOfClass:[NSArray class]], @"Stops should be NSArray");
        NSNumber *dx = offset[0];
        NSNumber *dy = offset[1];
        auto off = std::array<float, 2>({{dx.floatValue, dy.floatValue}});
        stops.emplace_back(zoomKey.floatValue, off);
    }];
    return mbgl::style::Function<std::array<float, 2>>({{stops}}, _base.floatValue);
}

+ (instancetype)functionWithColorPropertyValue:(mbgl::style::Function<mbgl::Color>)property
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    auto stops = property.getStops();
    NSMutableDictionary *convertedStops = [NSMutableDictionary dictionaryWithCapacity:stops.size()];
    for (auto stop : stops) {
        convertedStops[@(stop.first)] = [MGLColor mbgl_colorWithColor:stop.second];
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
        convertedStops[@(stop.first)] = @[@(stop.second[0]), @(stop.second[1])];
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
        convertedStops[@(stop.first)] = @[@(stop.second[0]), @(stop.second[1]), @(stop.second[2]), @(stop.second[3])];
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

@end
