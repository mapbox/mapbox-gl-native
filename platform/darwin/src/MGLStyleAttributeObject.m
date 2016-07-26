#import "MGLStyleAttributeObject.h"

#import <CoreGraphics/CoreGraphics.h>
#import "UIColor+MGLStyleAttributeAdditions.h"

@implementation MGLStyleAttributeObject

- (MGLAttributeColor)attributeColor
{
    return self.color.attributeColor;
}

@end
