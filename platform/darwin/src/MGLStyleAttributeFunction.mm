#import "MGLStyleAttributeFunction.h"

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue_Private.hpp"
#import "MGLStyleAttributeFunction_Private.hpp"

@implementation MGLStyleAttributeFunction

- (instancetype)init
{
    self = [super init];
    if (self == nil) return nil;
    _base = @1;
    return self;
}


- (mbgl::style::PropertyValue<mbgl::Color>)mbgl_colorPropertyValue
{
    std::vector<std::pair<float, mbgl::Color>> stops;
    
    NSArray *zoomKeys = self.stops.allKeys;
    for (NSNumber *zoomKey in zoomKeys) {
        UIColor *color = self.stops[zoomKey];
        stops.emplace_back(std::make_pair(zoomKey.floatValue, color.mbgl_color));
    }
    
    return mbgl::style::Function<mbgl::Color>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<float>)mbgl_floatPropertyValue
{
    std::vector<std::pair<float, float>> stops;
    
    NSArray *zoomKeys = self.stops.allKeys;
    for (NSNumber *zoomKey in zoomKeys) {
        NSNumber *number = self.stops[zoomKey];
        stops.emplace_back(std::make_pair(zoomKey.floatValue, number.floatValue));
    }
    
    return mbgl::style::Function<float>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<bool>)mbgl_boolPropertyValue
{
    std::vector<std::pair<float, bool>> stops;
    
    NSArray *zoomKeys = self.stops.allKeys;
    for (NSNumber *zoomKey in zoomKeys) {
        NSNumber *number = self.stops[zoomKey];
        stops.emplace_back(std::make_pair(zoomKey.floatValue, number.boolValue));
    }
    
    return mbgl::style::Function<bool>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<std::string>)mbgl_stringPropertyValue
{
    std::vector<std::pair<float, std::string>> stops;
    
    NSArray *zoomKeys = self.stops.allKeys;
    for (NSNumber *zoomKey in zoomKeys) {
        NSString *string = self.stops[zoomKey];
        stops.emplace_back(std::make_pair(zoomKey.floatValue, string.UTF8String));
    }
    
    return mbgl::style::Function<std::string>({{stops}}, _base.floatValue);
}

@end
