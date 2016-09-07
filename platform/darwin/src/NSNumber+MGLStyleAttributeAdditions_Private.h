#import <Foundation/Foundation.h>

#import "MGLStyleAttributeValue.h"

#include <mbgl/style/property_value.hpp>

@interface NSNumber (MGLStyleAttributeAdditions_Private) <MGLStyleAttributeValue>

- (mbgl::style::PropertyValue<bool>)mbgl_booleanPropertyValue;

- (mbgl::style::PropertyValue<float>)mbgl_numberPropertyValue;

@end
