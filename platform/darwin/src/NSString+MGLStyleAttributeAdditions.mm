#import "NSString+MGLStyleAttributeAdditions.h"

#import "NSString+MGLStyleAttributeAdditions_Private.h"

@implementation NSString (MGLStyleAttributeAdditions)

- (BOOL)isFunction
{
    return NO;
}

- (mbgl::style::PropertyValue<std::string>)mbgl_stringPropertyValue
{
    return mbgl::style::PropertyValue<std::string> {{ self.UTF8String }};
}

@end
