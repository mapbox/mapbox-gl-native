#import <Foundation/Foundation.h>

#import "MGLStyleAttributeValue.h"
#import "MGLTypes.h"

@interface MGLStyleAttributeObject : NSObject <MGLStyleAttributeValue>

@property (nonatomic) MGLColor *color;

@end
