#import <Cocoa/Cocoa.h>

#include <mbgl/util/color.hpp>

@interface NSColor (MGLAdditions)

- (mbgl::Color)mbgl_color;

@end
