#import <Foundation/Foundation.h>

#import "MGLStyleAttributeValue.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleAttributeValue_Private.h"

@interface MGLStyleAttribute : NSObject <MGLStyleAttributeValue>

+ (id <MGLStyleAttributeValue>)mgl_colorWithPropertyValueColor:(mbgl::style::PropertyValue<mbgl::Color>)property;

+ (id <MGLStyleAttributeValue>)mgl_numberWithPropertyValueNumber:(mbgl::style::PropertyValue<float>)property;

+ (id <MGLStyleAttributeValue>)mgl_boolWithPropertyValueBool:(mbgl::style::PropertyValue<bool>)property;

+ (id <MGLStyleAttributeValue>)mgl_stringWithPropertyValueString:(mbgl::style::PropertyValue<std::string>)property;

+ (id <MGLStyleAttributeValue>)mgl_offsetWithPropertyValueOffset:(mbgl::style::PropertyValue<std::array<float, 2>>)property;

+ (id <MGLStyleAttributeValue>)mgl_paddingWithPropertyValuePadding:(mbgl::style::PropertyValue<std::array<float, 4>>)property;

+ (id <MGLStyleAttributeValue>)mgl_stringArrayWithPropertyValueStringArray:(mbgl::style::PropertyValue<std::vector<std::string>>)property;

+ (id <MGLStyleAttributeValue>)mgl_numberArrayWithPropertyValueNumberArray:(mbgl::style::PropertyValue<std::vector<float>>)property;

@end
