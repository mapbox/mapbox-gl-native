#import <Cocoa/Cocoa.h>

#include <mbgl/util/color.hpp>

@interface NSColor (MGLAdditions)

/**
 Converts the color into an mbgl::Color in calibrated RGB space.
 */
- (mbgl::Color)mbgl_color;

/**
 Instantiates `NSColor` from an `mbgl::Color`
 */
+ (NSColor *)mbgl_color:(mbgl::Color)color;

@end
