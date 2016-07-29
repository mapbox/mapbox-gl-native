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
        if ([stop.t2 isKindOfClass:UIColor.class]) {
            NSNumber *t1 = stop.t1;
            UIColor *t2 = stop.t2; // TODO: t2 could be bool, float or color.
            stops.emplace_back(std::make_pair(t1.floatValue, t2.mbgl_color));
        }
    }
    
    return mbgl::style::Function<mbgl::Color>({{stops}}, _base.floatValue);
}

@end
