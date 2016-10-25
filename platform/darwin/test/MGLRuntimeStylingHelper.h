#import <Foundation/Foundation.h>

#import "MGLTypes.h"
#import "MGLStyleAttributeFunction.h"

@interface MGLRuntimeStylingHelper : NSObject

+ (NSArray *)testPadding;
+ (MGLStyleAttributeFunction *)testPaddingFunction;

+ (NSArray *)testOffset;
+ (MGLStyleAttributeFunction *)testOffsetFunction;

+ (NSArray *)testFont;
+ (MGLStyleAttributeFunction *)testFontFunction;

+ (NSArray *)testDashArray;
+ (MGLStyleAttributeFunction *)testDashArrayFunction;

+ (NSNumber *)testNumber;
+ (MGLStyleAttributeFunction *)testNumberFunction;

+ (NSNumber *)testBool;
+ (MGLStyleAttributeFunction *)testBoolFunction;

+ (NSString *)testString;
+ (MGLStyleAttributeFunction *)testStringFunction;

+ (MGLColor *)testColor;
+ (MGLStyleAttributeFunction *)testColorFunction;

+ (NSValue *)testEnum:(NSUInteger)value type:(const char *)type;
+ (MGLStyleAttributeFunction *)testEnumFunction:(NSUInteger)value type:(const char *)type;

@end
