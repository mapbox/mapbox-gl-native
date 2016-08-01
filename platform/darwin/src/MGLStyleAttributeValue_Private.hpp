#import <Foundation/Foundation.h>

#import "MGLTypes.h"
#import "MGLStyleAttributeFunction_Private.hpp"

@protocol MGLStyleAttributeValue_Private <NSObject>
- (BOOL)isFunction;
@optional
- (mbgl::style::PropertyValue<mbgl::Color>)mbgl_colorPropertyValue;
- (mbgl::style::PropertyValue<float>)mbgl_floatPropertyValue;
- (mbgl::style::PropertyValue<bool>)mbgl_boolPropertyValue;
- (mbgl::style::PropertyValue<std::string>)mbgl_stringPropertyValue;
@end
