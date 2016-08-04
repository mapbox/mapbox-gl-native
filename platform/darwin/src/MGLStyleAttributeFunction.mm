#import "MGLStyleAttributeFunction.h"

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue_Private.hpp"
#import "MGLStyleAttributeFunction_Private.hpp"

@implementation MGLStyleAttributeFunction

- (instancetype)init
{
    if (self = [super init]) {
        _base = @1;
    }
    return self;
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

+ (instancetype)functionWithColorPropertyValue:(mbgl::style::Function<mbgl::Color>)property
{
    auto stops = property.getStops();
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
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
    auto stops = property.getStops();
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
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
    auto stops = property.getStops();
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
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
    auto stops = property.getStops();
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
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
    auto stops = property.getStops();
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
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
    auto stops = property.getStops();
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
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
    auto stops = property.getStops();
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    NSMutableDictionary *convertedStops = [NSMutableDictionary dictionaryWithCapacity:stops.size()];
    for (auto stop : stops) {
        auto strings = stop.second;
        NSMutableArray *convertedStrings = [[NSMutableArray alloc] initWithCapacity:strings.size()];
        for (auto const& string: stop.second) {
            [convertedStrings addObject:@(string.c_str())];
        }
        convertedStops[@(stop.first)] = convertedStrings;
    }
    function.base = @(property.getBase());
    function.stops = convertedStops;
    return function;
}

@end
