#import "NSNumber+MGLStyleAttributeAdditions.h"

#include <mbgl/style/property_value.hpp>

@implementation NSNumber (MGLStyleAttributeAdditions)

- (NSNumber *)numberValue
{
    return self;
}

- (mbgl::style::PropertyValue<bool>)mbgl_boolPropertyValue
{
    return mbgl::style::PropertyValue<bool> { !!self.boolValue };
}

- (mbgl::style::PropertyValue<float>)mbgl_floatPropertyValue
{
    return mbgl::style::PropertyValue<float> { self.floatValue };
}

@end
