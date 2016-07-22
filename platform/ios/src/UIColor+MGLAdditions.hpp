#import <UIKit/UIKit.h>

#include <mbgl/util/color.hpp>

@interface UIColor (MGLAdditions)

- (mbgl::Color)mbgl_color;

+ (UIColor *)mbgl_color:(mbgl::Color)color;

@end
