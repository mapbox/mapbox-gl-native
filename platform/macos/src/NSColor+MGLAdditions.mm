#import "NSColor+MGLAdditions.h"

@implementation NSColor (MGLAdditions)

- (mbgl::Color)mbgl_color
{
    CGFloat r, g, b, a;
    
    [[self colorUsingColorSpaceName:NSCalibratedRGBColorSpace] getRed:&r green:&g blue:&b alpha:&a];
    
    return { (float)r, (float)g, (float)b, (float)a };
}

+ (NSColor *)mbgl_colorWithColor:(mbgl::Color)color
{
    return [NSColor colorWithRed:color.r green:color.g blue:color.b alpha:color.a];
}

- (mbgl::style::PropertyValue<mbgl::Color>)mbgl_colorPropertyValue
{
    mbgl::Color color = self.mbgl_color;
    return {{ color.r, color.g, color.b, color.a }};
}

@end
