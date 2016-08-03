#import <Foundation/Foundation.h>

#import "MGLStyleAttributeValue.h"
#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue_Private.hpp"

@interface MGLStyleAttribute : NSObject <MGLStyleAttributeValue>

+ (id <MGLStyleAttributeValue>)mbgl_colorPropertyValueWith:(mbgl::style::PropertyValue<mbgl::Color>)color;

@end
