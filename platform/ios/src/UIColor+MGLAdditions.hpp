#import <UIKit/UIKit.h>

#include <mbgl/util/color.hpp>
#include <mbgl/style/property_value.hpp>

@interface UIColor (MGLAdditions)

- (mbgl::Color)mbgl_color;

- (mbgl::style::PropertyValue<mbgl::Color>)mbgl_colorPropertyValue;

+ (UIColor *)mbgl_colorWithColor:(mbgl::Color)color;

@end
