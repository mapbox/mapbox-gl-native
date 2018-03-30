#import <Cocoa/Cocoa.h>

#include <mbgl/util/color.hpp>
#include <mbgl/style/property_value.hpp>

@interface NSColor (MGLAdditions)

/**
 Converts the color into an mbgl::Color in sRGB space.
 */
- (mbgl::Color)mgl_color;

/**
 Instantiates `NSColor` from an `mbgl::Color`
 */
+ (NSColor *)mgl_colorWithColor:(mbgl::Color)color;

- (mbgl::style::PropertyValue<mbgl::Color>)mgl_colorPropertyValue;

@end

@interface NSExpression (MGLColorAdditions)

+ (NSExpression *)mgl_expressionForRGBComponents:(NSArray<NSExpression *> *)components;
+ (NSExpression *)mgl_expressionForRGBAComponents:(NSArray<NSExpression *> *)components;

@end
