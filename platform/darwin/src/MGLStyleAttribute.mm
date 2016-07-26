#import "MGLStyleAttribute.hpp"
#import "UIColor+MGLStyleAttributeAdditions.h"
#import "UIColor+MGLAdditions.hpp"

@implementation MGLStyleAttribute

+ (mbgl::style::PropertyValue<mbgl::Color>)colorPropertyValueWith:(id<MGLStyleAttributeValue>)attributeValue
{
    return attributeValue.colorValue.mbgl_color;
}

@end
