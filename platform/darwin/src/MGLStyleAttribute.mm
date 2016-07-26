#import "MGLStyleAttribute.hpp"
#import "UIColor+MGLStyleAttributeAdditions.h"

@implementation MGLStyleAttribute

+ (mbgl::style::PropertyValue<mbgl::Color>)colorPropertyValueWith:(id<MGLStyleAttributeValue>)attributeValue
{
    MGLAttributeColor color = [attributeValue attributeColor];
    return mbgl::style::PropertyValue<mbgl::Color> {{ color.red, color.green, color.blue, color.alpha }};
}

@end
