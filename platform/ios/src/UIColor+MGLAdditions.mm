#import "UIColor+MGLAdditions.h"

@implementation UIColor (MGLAdditions)

- (mbgl::Color)mgl_color
{
    CGFloat r, g, b, a;
    [self getRed:&r green:&g blue:&b alpha:&a];
    return { (float)r, (float)g, (float)b, (float)a };
}

- (mbgl::style::PropertyValue<mbgl::Color>)mgl_colorPropertyValue
{
    mbgl::Color color = self.mgl_color;
    return {{ color.r, color.g, color.b, color.a }};
}

+ (UIColor *)mgl_colorWithColor:(mbgl::Color)color
{
    return [UIColor colorWithRed:color.r green:color.g blue:color.b alpha:color.a];
}

@end

@implementation NSExpression (MGLColorAdditions)

+ (NSExpression *)mgl_expressionForRGBComponents:(NSArray<NSExpression *> *)components {
    NSExpression *color = [NSExpression expressionForConstantValue:[UIColor class]];
    return [NSExpression expressionForFunction:color selectorName:@"colorWithRed:green:blue:" arguments:components];
}

+ (NSExpression *)mgl_expressionForRGBAComponents:(NSArray<NSExpression *> *)components {
    NSExpression *color = [NSExpression expressionForConstantValue:[UIColor class]];
    return [NSExpression expressionForFunction:color selectorName:@"colorWithRed:green:blue:alpha:" arguments:components];
}

@end
