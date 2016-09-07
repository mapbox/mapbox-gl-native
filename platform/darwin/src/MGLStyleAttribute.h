#import <Foundation/Foundation.h>

#import "MGLStyleAttributeValue.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleAttributeValue_Private.h"

@interface MGLStyleAttribute : NSObject <MGLStyleAttributeValue>

+ (id <MGLStyleAttributeValue>)mbgl_colorWithPropertyValueColor:(mbgl::style::PropertyValue<mbgl::Color>)property;

+ (id <MGLStyleAttributeValue>)mbgl_numberWithPropertyValueNumber:(mbgl::style::PropertyValue<float>)property;

+ (id <MGLStyleAttributeValue>)mbgl_boolWithPropertyValueBool:(mbgl::style::PropertyValue<bool>)property;

+ (id <MGLStyleAttributeValue>)mbgl_stringWithPropertyValueString:(mbgl::style::PropertyValue<std::string>)property;

+ (id <MGLStyleAttributeValue>)mbgl_offsetWithPropertyValueOffset:(mbgl::style::PropertyValue<std::array<float, 2>>)property;

+ (id <MGLStyleAttributeValue>)mbgl_paddingWithPropertyValuePadding:(mbgl::style::PropertyValue<std::array<float, 4>>)property;

+ (id <MGLStyleAttributeValue>)mbgl_stringArrayWithPropertyValueStringArray:(mbgl::style::PropertyValue<std::vector<std::string>>)property;

+ (id <MGLStyleAttributeValue>)mbgl_numberArrayWithPropertyValueNumberArray:(mbgl::style::PropertyValue<std::vector<float>>)property;

@end
