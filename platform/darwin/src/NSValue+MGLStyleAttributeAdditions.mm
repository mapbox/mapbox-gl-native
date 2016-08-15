#import "NSValue+MGLStyleAttributeAdditions.h"

#import "NSValue+MGLStyleAttributeAdditions_Private.h"

@implementation NSValue (MGLStyleAttributeAdditions)

- (BOOL)isFunction
{
    return NO;
}

- (mbgl::style::PropertyValue<uint8_t>)mbgl_enumPropertyValue
{
    uint8_t value = 0;
    [self getValue:&value];
    return mbgl::style::PropertyValue<uint8_t> { value };
}

@end
