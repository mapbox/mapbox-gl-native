#import <Foundation/Foundation.h>

#import "MGLTypes.h"

@protocol MGLStyleAttributeValue <NSObject>
@optional
- (NSString *)stringValue;
- (MGLColor *)colorValue;
- (NSNumber *)numberValue;
- (NSValue *)enumValue;
@end