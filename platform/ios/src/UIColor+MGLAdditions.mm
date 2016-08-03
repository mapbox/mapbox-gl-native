#import "UIColor+MGLAdditions.hpp"

#import "MGLStyleAttributeValue.h"
#import "MGLStyleAttributeValue_Private.hpp"

@implementation UIColor (MGLAdditions)

- (mbgl::Color)mbgl_color
{
    CGFloat r, g, b, a;
    [self getRed:&r green:&g blue:&b alpha:&a];
    return { (float)r, (float)g, (float)b, (float)a };
}

- (mbgl::style::PropertyValue<mbgl::Color>)mbgl_colorPropertyValue
{
    mbgl::Color color = self.mbgl_color;
    return {{ color.r, color.g, color.b, color.a }};
}

+ (UIColor *)mbgl_colorWithColor:(mbgl::Color)color
{
    return [UIColor colorWithRed:color.r green:color.g blue:color.b alpha:color.a];
}

+ (UIColor *)mbgl_propertyValue:(mbgl::style::PropertyValue<mbgl::Color>)color
{
#warning Figure out if property value is constant, undefined or a function.
    return [UIColor colorWithRed:color.asConstant().r green:color.asConstant().g blue:color.asConstant().b alpha:color.asConstant().a];
}

@end
