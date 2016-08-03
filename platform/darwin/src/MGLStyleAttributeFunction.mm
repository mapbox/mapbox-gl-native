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

@end
