#import <Foundation/Foundation.h>

#import "MGLTypes.h"
#import "MGLStyleAttributeFunction_Private.hpp"
#include <array>

@protocol MGLStyleAttributeValue_Private <NSObject>
- (BOOL)isFunction;
@optional
- (mbgl::style::PropertyValue<mbgl::Color>)mbgl_colorPropertyValue;
- (mbgl::style::PropertyValue<float>)mbgl_floatPropertyValue;
- (mbgl::style::PropertyValue<bool>)mbgl_boolPropertyValue;
- (mbgl::style::PropertyValue<std::string>)mbgl_stringPropertyValue;
- (mbgl::style::PropertyValue<std::array<float, 2>>)mbgl_offsetPropertyValue;
- (mbgl::style::PropertyValue<std::array<float, 4>>)mbgl_paddingPropertyValue;
- (mbgl::style::PropertyValue<std::vector<std::string> >)mbgl_stringArrayPropertyValue;
- (mbgl::style::PropertyValue<std::vector<float> >)mbgl_numberArrayPropertyValue;
- (mbgl::style::PropertyValue<uint8_t>)mbgl_enumPropertyValue;

@end
