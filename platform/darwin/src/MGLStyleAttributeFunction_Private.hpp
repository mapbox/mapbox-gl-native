#import "MGLStyleAttributeFunction.h"

#import <mbgl/util/color.hpp>
#include <mbgl/style/function.hpp>
#include <mbgl/style/property_value.hpp>

@interface MGLStyleAttributeFunction(Private)
- (mbgl::style::Function<mbgl::Color>)colorFunction;
@end;