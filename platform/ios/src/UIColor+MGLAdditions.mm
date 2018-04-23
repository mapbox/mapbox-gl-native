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
    if (UIColor *color = [self mgl_colorWithRGBComponents:components]) {
        return [NSExpression expressionForConstantValue:color];
    }
    
    NSExpression *color = [NSExpression expressionForConstantValue:[UIColor class]];
    NSExpression *alpha = [NSExpression expressionForConstantValue:@1.0];
    return [NSExpression expressionForFunction:color
                                  selectorName:@"colorWithRed:green:blue:alpha:"
                                     arguments:[components arrayByAddingObject:alpha]];
}

+ (NSExpression *)mgl_expressionForRGBAComponents:(NSArray<NSExpression *> *)components {
    if (UIColor *color = [self mgl_colorWithRGBComponents:components]) {
        return [NSExpression expressionForConstantValue:color];
    }
    
    NSExpression *color = [NSExpression expressionForConstantValue:[UIColor class]];
    return [NSExpression expressionForFunction:color
                                  selectorName:@"colorWithRed:green:blue:alpha:"
                                     arguments:components];
}

+ (UIColor *)mgl_colorWithRGBComponents:(NSArray<NSExpression *> *)components {
    if (components.count < 3 || components.count > 4) {
        return nil;
    }
    
    for (NSExpression *component in components) {
        if (component.expressionType != NSConstantValueExpressionType) {
            return nil;
        }
        
        NSNumber *number = (NSNumber *)component.constantValue;
        if (![number isKindOfClass:[NSNumber class]]) {
            return nil;
        }
    }
    
    return [UIColor colorWithRed:[components[0].constantValue doubleValue] / 255.0
                           green:[components[1].constantValue doubleValue] / 255.0
                            blue:[components[2].constantValue doubleValue] / 255.0
                           alpha:components.count == 3 ? [components[3].constantValue doubleValue] : 1.0];
}

@end
