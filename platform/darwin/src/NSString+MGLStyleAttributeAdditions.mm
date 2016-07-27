#import "NSString+MGLStyleAttributeAdditions.h"

#import "NSString+MGLStyleAttributeAdditions_Private.hpp"

@implementation NSString (MGLStyleAttributeAdditions)

- (mbgl::style::PropertyValue<std::string>)mbgl_stringPropertyValue
{
    return mbgl::style::PropertyValue<std::string> {{ self.UTF8String }};
}

+ (NSString *)mbgl_stringWithPropertyValue:(mbgl::style::PropertyValue<std::string>)propertyValue
{
    // TODO: Figure out if property is a constant, undefined or a function.
    return @(propertyValue.asConstant().c_str());
}

@end
