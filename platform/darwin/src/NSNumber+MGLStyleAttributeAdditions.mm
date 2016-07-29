#import "NSNumber+MGLStyleAttributeAdditions.h"

#include <mbgl/style/property_value.hpp>

@implementation NSNumber (MGLStyleAttributeAdditions)

- (NSNumber *)numberValue
{
    return self;
}

- (mbgl::style::PropertyValue<bool>)mbgl_booleanPropertyValue
{
    // TODO: Figure out if value is constant, undefined or a function
    return mbgl::style::PropertyValue<bool> { self.boolValue };
}

- (mbgl::style::PropertyValue<float>)mbgl_numberPropertyValue
{
    // TODO: Figure out if value is constant, undefined or a function
    return mbgl::style::PropertyValue<float> { self.floatValue };
}

+ (NSNumber *)mbgl_booleanWithPropertyValue:(mbgl::style::PropertyValue<bool>)propertyValue
{
    // TODO: Figure out if value is constant, undefined or a function
    return @(propertyValue.asConstant());
}

+ (NSNumber *)mbgl_numberWithPropertyValue:(mbgl::style::PropertyValue<float>)propertyValue
{
    // TODO: Figure out if value is constant, undefined or a function
    return @(propertyValue.asConstant());
}

@end
