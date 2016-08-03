#import <Foundation/Foundation.h>

#import "MGLStyleAttributeValue.h"
#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue_Private.hpp"

@interface MGLStyleAttribute : NSObject <MGLStyleAttributeValue>

+ (id <MGLStyleAttributeValue>)mbgl_colorPropertyValueWith:(mbgl::style::PropertyValue<mbgl::Color>)property;

+ (id <MGLStyleAttributeValue>)mbgl_numberPropertyValueWith:(mbgl::style::PropertyValue<float>)property;

+ (id <MGLStyleAttributeValue>)mbgl_boolPropertyValueWith:(mbgl::style::PropertyValue<bool>)property;

@end
