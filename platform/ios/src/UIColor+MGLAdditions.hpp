#import <UIKit/UIKit.h>

#include <mbgl/util/color.hpp>

@interface UIColor (MGLAdditions)

- (mbgl::Color)mbgl_color;

@end
