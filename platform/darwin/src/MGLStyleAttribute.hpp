#import <Foundation/Foundation.h>

#import "MGLStyleAttributeValue.h"
#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue_Private.hpp"

@interface MGLStyleAttribute : NSObject <MGLStyleAttributeValue>

+ (id <MGLStyleAttributeValue>)mbgl_colorPropertyValueWith:(mbgl::style::PropertyValue<mbgl::Color>)property;

+ (id <MGLStyleAttributeValue>)mbgl_numberPropertyValueWith:(mbgl::style::PropertyValue<float>)property;

+ (id <MGLStyleAttributeValue>)mbgl_boolPropertyValueWith:(mbgl::style::PropertyValue<bool>)property;

+ (id <MGLStyleAttributeValue>)mbgl_stringPropertyValueWith:(mbgl::style::PropertyValue<std::string>)property;

+ (id <MGLStyleAttributeValue>)mbgl_offsetPropertyValueWith:(mbgl::style::PropertyValue<std::array<float, 2>>)property;

+ (id <MGLStyleAttributeValue>)mbgl_paddingPropertyValueWith:(mbgl::style::PropertyValue<std::array<float, 4>>)property;

+ (id <MGLStyleAttributeValue>)mbgl_stringArrayPropertyValueWith:(mbgl::style::PropertyValue<std::vector<std::string>>)property;

@end
