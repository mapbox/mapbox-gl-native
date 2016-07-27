#import <Foundation/Foundation.h>

#import "MGLStyleAttributeValue.h"

#include <mbgl/style/property_value.hpp>

@interface NSNumber (MGLStyleAttributeAdditions_Private) <MGLStyleAttributeValue>

- (mbgl::style::PropertyValue<bool>)mbgl_booleanPropertyValue;

+ (NSNumber *)mbgl_booleanWithPropertyValue:(mbgl::style::PropertyValue<bool>)propertyValue;

+ (NSNumber *)mbgl_numberWithPropertyValue:(mbgl::style::PropertyValue<float>)propertyValue;

@end
