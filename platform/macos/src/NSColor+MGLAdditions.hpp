#import <Cocoa/Cocoa.h>

#include <mbgl/util/color.hpp>
#include <mbgl/style/property_value.hpp>

@interface NSColor (MGLAdditions)

/**
 Converts the color into an mbgl::Color in calibrated RGB space.
 */
- (mbgl::Color)mbgl_color;

/**
 Instantiates `NSColor` from an `mbgl::Color`
 */
+ (NSColor *)mbgl_colorWithColor:(mbgl::Color)color;

- (mbgl::style::PropertyValue<mbgl::Color>)mbgl_colorPropertyValue;

@end
