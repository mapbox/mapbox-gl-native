#import "UIColor+MGLStyleAttributeAdditions.h"

#import "MGLStyleAttributeValue.h"

@implementation UIColor (MGLStyleAttributeAdditions)

- (BOOL)isFunction
{
    return NO;
}

- (UIColor *)colorValue
{
    return self;
}

@end
