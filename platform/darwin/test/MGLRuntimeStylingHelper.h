#import <Foundation/Foundation.h>

#import "MGLTypes.h"
#import "MGLStyleValue.h"

@interface MGLRuntimeStylingHelper : NSObject

+ (MGLStyleConstantValue<NSValue *> *)testPadding;
+ (MGLStyleFunction<NSValue *> *)testPaddingFunction;

+ (MGLStyleConstantValue<NSValue *> *)testOffset;
+ (MGLStyleFunction<NSValue *> *)testOffsetFunction;

+ (MGLStyleConstantValue<NSArray<NSString *> *> *)testFont;
+ (MGLStyleFunction<NSArray<NSString *> *> *)testFontFunction;

+ (MGLStyleConstantValue<NSArray<NSNumber *> *> *)testDashArray;
+ (MGLStyleFunction<NSArray<NSNumber *> *> *)testDashArrayFunction;

+ (MGLStyleConstantValue<NSNumber *> *)testNumber;
+ (MGLStyleFunction<NSNumber *> *)testNumberFunction;

+ (MGLStyleConstantValue<NSNumber *> *)testBool;
+ (MGLStyleFunction<NSNumber *> *)testBoolFunction;

+ (MGLStyleConstantValue<NSString *> *)testString;
+ (MGLStyleFunction<NSString *> *)testStringFunction;

+ (MGLStyleConstantValue<MGLColor *> *)testColor;
+ (MGLStyleFunction<MGLColor *> *)testColorFunction;

+ (MGLStyleConstantValue<NSValue *> *)testEnum:(NSUInteger)value type:(const char *)type;
+ (MGLStyleFunction<NSValue *> *)testEnumFunction:(NSUInteger)value type:(const char *)type;

@end
