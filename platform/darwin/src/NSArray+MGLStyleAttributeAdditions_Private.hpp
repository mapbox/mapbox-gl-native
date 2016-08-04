#import <Foundation/Foundation.h>

#import "MGLStyleAttributeValue.h"

#include <mbgl/style/property_value.hpp>

@interface NSArray (MGLStyleAttributeAdditions_Private) <MGLStyleAttributeValue>

+ (NSArray *)mbgl_numberArrayPropertyValue:(mbgl::style::PropertyValue<std::vector<float> >)propertyValue;

@end
