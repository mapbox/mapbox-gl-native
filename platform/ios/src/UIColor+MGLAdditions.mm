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

@end
