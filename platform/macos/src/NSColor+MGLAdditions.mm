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

    // NSColor provides non-premultiplied color components, so we have to premultiply each
    // color component with the alpha value to transform it into a valid
    // mbgl::Color which expects premultiplied color components.
    return { static_cast<float>(r*a), static_cast<float>(g*a), static_cast<float>(b*a), static_cast<float>(a) };
}

+ (NSColor *)mgl_colorWithColor:(mbgl::Color)color {
    // If there is no alpha value, return original color values.
    if (color.a == 0.0f) {
        // macOS 10.12 Sierra and below uses calibrated RGB by default.
        if ([NSColor redColor].colorSpaceName == NSCalibratedRGBColorSpace) {
            return [NSColor colorWithCalibratedRed:color.r green:color.g blue:color.b alpha:color.a];
        } else {
            return [NSColor colorWithRed:color.r green:color.g blue:color.b alpha:color.a];
        }
    }
    
    // mbgl::Color provides premultiplied color components, so we have to convert color
    // components to non-premultiplied values to return a valid NSColor object.
    float red = static_cast<float>((color.r / color.a));
    float green = static_cast<float>((color.g / color.a));
    float blue = static_cast<float>((color.b / color.a));

    // macOS 10.12 Sierra and below uses calibrated RGB by default.
    if ([NSColor redColor].colorSpaceName == NSCalibratedRGBColorSpace) {
        return [NSColor colorWithCalibratedRed:red green:green blue:blue alpha:color.a];
    } else {
        return [NSColor colorWithRed:red green:green blue:blue alpha:color.a];
    }
}

- (mbgl::style::PropertyValue<mbgl::Color>)mgl_colorPropertyValue {
    mbgl::Color color = self.mgl_color;
    return {{ color.r, color.g, color.b, color.a }};
}

@end

@implementation NSExpression (MGLColorAdditions)

+ (NSExpression *)mgl_expressionForRGBComponents:(NSArray<NSExpression *> *)components {
    if (NSColor *color = [self mgl_colorWithRGBComponents:components]) {
        return [NSExpression expressionForConstantValue:color];
    }
    
    NSExpression *color = [NSExpression expressionForConstantValue:[NSColor class]];
    NSExpression *alpha = [NSExpression expressionForConstantValue:@1.0];
    return [NSExpression expressionForFunction:color
                                  selectorName:@"colorWithRed:green:blue:alpha:"
                                     arguments:[components arrayByAddingObject:alpha]];
}

+ (NSExpression *)mgl_expressionForRGBAComponents:(NSArray<NSExpression *> *)components {
    if (NSColor *color = [self mgl_colorWithRGBComponents:components]) {
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
+ (NSColor *)mgl_colorWithRGBComponents:(NSArray<NSExpression *> *)componentExpressions {
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
    
    if (components.size() < 4) {
        components.push_back(1.0);
    } else {
        // Alpha
        components.back() *= 255.0;
    }
    
    
    // macOS 10.12 Sierra and below uses calibrated RGB by default.
    if ([NSColor redColor].colorSpaceName == NSCalibratedRGBColorSpace) {
        return [NSColor colorWithCalibratedRed:components[0]
                                         green:components[1]
                                          blue:components[2]
                                         alpha:components[3]];
    }
    // The Mapbox Style Specification does not specify a color space, but it is
    // assumed to be sRGB for consistency with CSS.
    return [NSColor colorWithColorSpace:[NSColorSpace sRGBColorSpace]
                             components:&components[0]
                                  count:components.size()];
}

@end
