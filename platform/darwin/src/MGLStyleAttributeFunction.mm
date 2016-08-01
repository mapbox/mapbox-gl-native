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
    
    for (MGLStyleAttributePair *stop in self.stops) {
        NSNumber *t1 = stop.t1;
        MGLColor *t2 = stop.t2;
        stops.emplace_back(std::make_pair(t1.floatValue, t2.mbgl_color));
    }
    
    return mbgl::style::Function<mbgl::Color>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<float>)mbgl_floatPropertyValue
{
    std::vector<std::pair<float, float>> stops;
    
    for (MGLStyleAttributePair *stop in self.stops) {
        NSNumber *t1 = stop.t1;
        NSNumber *t2 = stop.t2;
        stops.emplace_back(std::make_pair(t1.floatValue, t2.floatValue));
    }
    
    return mbgl::style::Function<float>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<bool>)mbgl_boolPropertyValue
{
    std::vector<std::pair<float, bool>> stops;
    
    for (MGLStyleAttributePair *stop in self.stops) {
        NSNumber *t1 = stop.t1;
        NSNumber *t2 = stop.t2;
        stops.emplace_back(std::make_pair(t1.floatValue, t2.boolValue));
    }
    
    return mbgl::style::Function<bool>({{stops}}, _base.floatValue);
}

- (mbgl::style::PropertyValue<std::string>)mbgl_stringPropertyValue
{
    std::vector<std::pair<float, std::string>> stops;
    
    for (MGLStyleAttributePair *stop in self.stops) {
        NSNumber *t1 = stop.t1;
        NSString *t2 = stop.t2;
        stops.emplace_back(std::make_pair(t1.floatValue, t2.UTF8String));
    }
    
    return mbgl::style::Function<std::string>({{stops}}, _base.floatValue);
}

@end
