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
    if (NSColor *color = [self mgl_colorWithComponentExpressions:components]) {
        return [NSExpression expressionForConstantValue:color];
    }
    
    NSExpression *color = [NSExpression expressionForConstantValue:[NSColor class]];
    NSExpression *alpha = [NSExpression expressionForConstantValue:@1.0];
    return [NSExpression expressionForFunction:color
                                  selectorName:@"colorWithRed:green:blue:alpha:"
                                     arguments:[components arrayByAddingObject:alpha]];
}

+ (NSExpression *)mgl_expressionForRGBAComponents:(NSArray<NSExpression *> *)components {
    if (NSColor *color = [self mgl_colorWithComponentExpressions:components]) {
        return [NSExpression expressionForConstantValue:color];
    }
    
    NSExpression *color = [NSExpression expressionForConstantValue:[NSColor class]];
    return [NSExpression expressionForFunction:color
                                  selectorName:@"colorWithRed:green:blue:alpha:"
                                     arguments:components];
}

/**
 Returns a color object corresponding to the given component expressions.
 */
+ (NSColor *)mgl_colorWithComponentExpressions:(NSArray<NSExpression *> *)componentExpressions {
    // Map the component expressions to constant components. If any component is
    // a non-constant expression, the components cannot be converted into a
    // constant color value.
    std::vector<CGFloat> components;
    for (NSExpression *componentExpression in componentExpressions) {
        if (componentExpression.expressionType != NSConstantValueExpressionType) {
            return nil;
        }
        
        NSNumber *component = (NSNumber *)componentExpression.constantValue;
        if (![component isKindOfClass:[NSNumber class]]) {
            return nil;
        }
        
        components.push_back(component.doubleValue / 255.0);
    }
    // Alpha
    components.back() *= 255.0;
    
    // The Mapbox Style Specification does not specify a color space, but it is
    // assumed to be sRGB for consistency with CSS.
    return [NSColor colorWithColorSpace:[NSColorSpace sRGBColorSpace]
                             components:&components[0]
                                  count:components.size()];
}

@end
