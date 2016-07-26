#import <UIKit/UIKit.h>

#import "MGLStyleAttributeValue.h"

@interface UIColor (MGLStyleAttributeAdditions) <MGLStyleAttributeValue>

- (id<MGLStyleAttributeValue>)mgl_colorAttributeValue;

- (MGLAttributeColor)attributeColor;

@end
