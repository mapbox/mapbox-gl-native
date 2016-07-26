#import "UIColor+MGLStyleAttributeAdditions.h"

#import "MGLStyleAttributeValue.h"
#import "MGLStyleAttributeObject.h"

@implementation UIColor (MGLStyleAttributeAdditions)

- (id<MGLStyleAttributeValue>)mgl_colorAttributeValue
{
    MGLStyleAttributeObject *obj = [[MGLStyleAttributeObject alloc] init];
    obj.color = self;
    return obj;
}

- (MGLAttributeColor)attributeColor
{
    CGFloat r, g, b, a;
    [self getRed:&r green:&g blue:&b alpha:&a];
    
    MGLAttributeColor color;
    color.red = r;
    color.green = g;
    color.blue = b;
    color.alpha = a;
    return color;
}

@end
