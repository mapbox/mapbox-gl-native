#import "NSNumber+MGLStyleAttributeAdditions.h"

#include <mbgl/style/property_value.hpp>

@implementation NSNumber (MGLStyleAttributeAdditions)

- (NSNumber *)numberValue
{
    return self;
}

- (mbgl::style::PropertyValue<bool>)mgl_boolPropertyValue
{
    return mbgl::style::PropertyValue<bool> { !!self.boolValue };
}

- (mbgl::style::PropertyValue<float>)mgl_floatPropertyValue
{
    return mbgl::style::PropertyValue<float> { self.floatValue };
}

@end
