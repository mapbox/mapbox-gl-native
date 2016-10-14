#import <Foundation/Foundation.h>

#import "MGLStyleAttributeFunction_Private.h"

#include <array>

@protocol MGLStyleAttributeValue_Private <NSObject>
- (BOOL)isFunction;
@optional

// Convert darwin types to mbgl types
- (mbgl::style::PropertyValue<mbgl::Color>)mgl_colorPropertyValue;
- (mbgl::style::PropertyValue<float>)mgl_floatPropertyValue;
- (mbgl::style::PropertyValue<bool>)mgl_boolPropertyValue;
- (mbgl::style::PropertyValue<std::string>)mgl_stringPropertyValue;
- (mbgl::style::PropertyValue<std::array<float, 2>>)mgl_offsetPropertyValue;
- (mbgl::style::PropertyValue<std::array<float, 4>>)mgl_paddingPropertyValue;
- (mbgl::style::PropertyValue<std::vector<std::string> >)mgl_stringArrayPropertyValue;
- (mbgl::style::PropertyValue<std::vector<float> >)mgl_numberArrayPropertyValue;

// Convert mbgl types to darwin types
- (id <MGLStyleAttributeValue>)mgl_colorPropertyValueWith:(mbgl::style::PropertyValue<mbgl::Color>)color;

@end
