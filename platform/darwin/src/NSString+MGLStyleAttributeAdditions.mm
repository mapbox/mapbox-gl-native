#import "NSString+MGLStyleAttributeAdditions.h"

#import "NSString+MGLStyleAttributeAdditions_Private.hpp"

@implementation NSString (MGLStyleAttributeAdditions)

- (mbgl::style::PropertyValue<std::string>)mbgl_stringPropertyValue
{
    return mbgl::style::PropertyValue<std::string> {{ self.UTF8String }};
}

@end
