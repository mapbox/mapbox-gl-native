#import <UIKit/UIKit.h>

#include <mbgl/util/color.hpp>
#include <mbgl/style/property_value.hpp>

@interface UIColor (MGLAdditions)

- (mbgl::Color)mbgl_color;

- (mbgl::style::PropertyValue<mbgl::Color>)mbgl_colorPropertyValue;

+ (UIColor *)mbgl_colorWithColor:(mbgl::Color)color;

+ (UIColor *)mbgl_propertyValue:(mbgl::style::PropertyValue<mbgl::Color>)color;

@end
