#import <Foundation/Foundation.h>

#import "MGLStyleAttributeValue.h"

#include <mbgl/style/property_value.hpp>

@interface NSString (MGLStyleAttributeAdditions_Private) <MGLStyleAttributeValue>

- (mbgl::style::PropertyValue<std::string>)mbgl_stringPropertyValue;

+ (NSString *)mbgl_stringWithPropertyValue:(mbgl::style::PropertyValue<std::string>)propertyValue;

@end
