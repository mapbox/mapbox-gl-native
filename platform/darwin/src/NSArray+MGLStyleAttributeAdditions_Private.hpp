#import <Foundation/Foundation.h>

#import "MGLStyleAttributeValue.h"

#include <mbgl/style/property_value.hpp>

@interface NSArray (MGLStyleAttributeAdditions_Private) <MGLStyleAttributeValue>

+ (NSArray *)mbgl_offsetPropertyValue:(mbgl::style::PropertyValue<std::array<float, 2> >)propertyValue;
+ (NSArray *)mbgl_paddingPropertyValue:(mbgl::style::PropertyValue<std::array<float, 4> >)propertyValue;
+ (NSArray *)mbgl_numberArrayPropertyValue:(mbgl::style::PropertyValue<std::vector<float> >)propertyValue;
+ (NSArray *)mbgl_stringArrayPropertyValue:(mbgl::style::PropertyValue<std::vector<std::string> >)propertyValue;

@end
