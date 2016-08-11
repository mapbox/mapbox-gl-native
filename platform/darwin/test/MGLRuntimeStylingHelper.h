#import <Foundation/Foundation.h>

#import "MGLTypes.h"

@interface MGLRuntimeStylingHelper : NSObject

+ (NSArray *)testPadding;

+ (NSArray *)testOffset;

+ (NSArray *)testFont;

+ (NSArray *)testDashArray;

+ (NSNumber *)testNumber;

+ (NSNumber *)testBool;

+ (NSString *)testString;

+ (MGLColor *)testColor;

+ (NSValue *)testEnum:(NSUInteger)value type:(const char *)type;

@end
