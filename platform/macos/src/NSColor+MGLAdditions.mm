#import "NSColor+MGLAdditions.h"

@implementation NSColor (MGLAdditions)

- (mbgl::Color)mgl_color {
    CGFloat r, g, b, a;

    // The Mapbox Style Specification does not specify a color space, but it is
    // assumed to be sRGB for consistency with CSS.
    NSColor *srgbColor = self;
    if ([NSColor redColor].colorSpaceName == NSCalibratedRGBColorSpace) {
        srgbColor = [srgbColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace];
    } else {
        srgbColor = [srgbColor colorUsingColorSpace:[NSColorSpace sRGBColorSpace]];
    }
    [srgbColor getRed:&r green:&g blue:&b alpha:&a];

    return { (float)r, (float)g, (float)b, (float)a };
}

+ (NSColor *)mgl_colorWithColor:(mbgl::Color)color {
    // macOS 10.12 Sierra and below uses calibrated RGB by default.
    if ([NSColor redColor].colorSpaceName == NSCalibratedRGBColorSpace) {
        return [NSColor colorWithCalibratedRed:color.r green:color.g blue:color.b alpha:color.a];
    } else {
        return [NSColor colorWithRed:color.r green:color.g blue:color.b alpha:color.a];
    }
}

- (mbgl::style::PropertyValue<mbgl::Color>)mgl_colorPropertyValue {
    mbgl::Color color = self.mgl_color;
    return {{ color.r, color.g, color.b, color.a }};
}

@end

@implementation NSExpression (MGLColorAdditions)

+ (NSExpression *)mgl_expressionForRGBComponents:(NSArray<NSExpression *> *)components {
    NSExpression *color = [NSExpression expressionForConstantValue:[NSColor class]];
    return [NSExpression expressionForFunction:color selectorName:@"colorWithCalibratedRed:green:blue:" arguments:components];
}

+ (NSExpression *)mgl_expressionForRGBAComponents:(NSArray<NSExpression *> *)components {
    NSExpression *color = [NSExpression expressionForConstantValue:[NSColor class]];
    return [NSExpression expressionForFunction:color selectorName:@"colorWithCalibratedRed:green:blue:alpha:" arguments:components];
}

@end
