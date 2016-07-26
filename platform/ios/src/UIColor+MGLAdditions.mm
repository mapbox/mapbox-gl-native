#import "UIColor+MGLAdditions.hpp"

@implementation UIColor (MGLAdditions)

- (mbgl::Color)mbgl_color
{
    if (!self)
    {
        return { 0, 0, 0, 0 };
    }
    CGFloat r, g, b, a;
    [self getRed:&r green:&g blue:&b alpha:&a];
    
    return { (float)r, (float)g, (float)b, (float)a };
}

- (mbgl::style::PropertyValue<mbgl::Color>)mbgl_propertyValue
{
    // TODO: Figure out if the value is a constant, undefined or a function.
    mbgl::Color color = self.mbgl_color;
    return {{ color.r, color.g, color.b, color.a }};
}

+ (UIColor *)mbgl_color:(mbgl::Color)color
{
    return [UIColor colorWithRed:color.r green:color.g blue:color.b alpha:color.a];
}

+ (UIColor *)mbgl_propertyValue:(mbgl::style::PropertyValue<mbgl::Color>)color
{
    // TODO: Figure out if property value is constant, undefined or a function.
    return [UIColor colorWithRed:color.asConstant().r
                           green:color.asConstant().g
                            blue:color.asConstant().b
                           alpha:color.asConstant().a];
}

@end
