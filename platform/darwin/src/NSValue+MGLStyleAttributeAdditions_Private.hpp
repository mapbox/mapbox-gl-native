#import <Foundation/Foundation.h>

#import "MGLStyleAttributeValue.h"

#include <mbgl/style/property_value.hpp>

@interface NSValue (MGLStyleAttributeAdditions_Private) <MGLStyleAttributeValue>
- (mbgl::style::PropertyValue<uint8_t>)mbgl_enumPropertyValue;
@end
