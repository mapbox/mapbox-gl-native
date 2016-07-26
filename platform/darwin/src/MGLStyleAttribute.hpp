#import <Foundation/Foundation.h>

#import "MGLStyleAttributeValue.h"

#include <mbgl/util/color.hpp>
#include <mbgl/style/property_value.hpp>


@interface MGLStyleAttribute : NSObject <MGLStyleAttributeValue>

+ (mbgl::style::PropertyValue<mbgl::Color>)colorPropertyValueWith:(id <MGLStyleAttributeValue>)attributeValue;

@end
