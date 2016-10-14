#import <Foundation/Foundation.h>

#import "MGLStyleAttributeValue.h"

#include <mbgl/style/property_value.hpp>

@interface NSString (MGLStyleAttributeAdditions_Private) <MGLStyleAttributeValue>

- (mbgl::style::PropertyValue<std::string>)mgl_stringPropertyValue;

@end
