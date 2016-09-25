#import "MGLRuntimeStylingHelper.h"

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
    #import <UIKit/UIKit.h>
    #define MGLEdgeInsets UIEdgeInsets
#else
    #import <Cocoa/Cocoa.h>
    #define MGLEdgeInsets NSEdgeInsets
#endif

@implementation MGLRuntimeStylingHelper

+ (MGLStyleConstantValue<NSValue *> *)testPadding
{
    MGLEdgeInsets insets = {
        .top = 1,
        .left = 1,
        .bottom = 1,
        .right = 1,
    };
    return [MGLStyleConstantValue<NSValue *> valueWithRawValue:[NSValue value:&insets withObjCType:@encode(MGLEdgeInsets)]];
}

+ (MGLStyleFunction<NSValue *> *)testPaddingFunction
{
    return [MGLStyleFunction<NSValue *> functionWithStops:@{@(18): self.testPadding}];
}

+ (MGLStyleConstantValue<NSValue *> *)testOffset
{
    CGVector vector = CGVectorMake(1, 1);
    return [MGLStyleConstantValue<NSValue *> valueWithRawValue:[NSValue value:&vector withObjCType:@encode(CGVector)]];
}

+ (MGLStyleFunction<NSValue *> *)testOffsetFunction
{
    return [MGLStyleFunction<NSValue *> valueWithStops:@{ @(18): self.testOffset }];
}

+ (MGLStyleConstantValue<NSArray<NSString *> *> *)testFont
{
    return [MGLStyleConstantValue<NSArray<NSString *> *> valueWithRawValue:@[@"Open Sans Regular", @"Arial Unicode MS Regular"]];
}

+ (MGLStyleFunction<NSArray<NSString *> *> *)testFontFunction
{
    return [MGLStyleFunction<NSArray<NSString *> *> valueWithStops:@{ @18: self.testFont }];
}

+ (MGLStyleConstantValue<NSArray<NSNumber *> *> *)testDashArray
{
    return [MGLStyleConstantValue<NSArray<NSNumber *> *> valueWithRawValue:@[@1, @2]];
}

+ (MGLStyleFunction<NSArray<NSNumber *> *> *)testDashArrayFunction
{
    return [MGLStyleFunction<NSArray<NSNumber *> *> valueWithStops:@{
        @18: self.testDashArray,
    }];
}

+ (MGLStyleConstantValue<NSNumber *> *)testNumber
{
    return [MGLStyleConstantValue<NSNumber *> valueWithRawValue:@1];
}

+ (MGLStyleFunction<NSNumber *> *)testNumberFunction
{
    return [MGLStyleFunction<NSNumber *> valueWithStops:@{
        @18: self.testNumber,
    }];
}

+ (MGLStyleConstantValue<NSNumber *> *)testBool
{
    return [MGLStyleConstantValue<NSNumber *> valueWithRawValue:@YES];
}

+ (MGLStyleFunction<NSNumber *> *)testBoolFunction
{
    return [MGLStyleFunction<NSNumber *> valueWithStops:@{
        @18: self.testBool,
    }];
}

+ (MGLStyleConstantValue<NSString *> *)testString
{
    return [MGLStyleConstantValue<NSString *> valueWithRawValue:@"test"];
}

+ (MGLStyleFunction<NSString *> *)testStringFunction
{
    return [MGLStyleFunction<NSString *> valueWithStops:@{
        @18: self.testString,
    }];
}

+ (MGLStyleConstantValue<MGLColor *> *)testColor
{
    return [MGLStyleConstantValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
}

+ (MGLStyleFunction<MGLColor *> *)testColorFunction
{
    return [MGLStyleFunction<MGLColor *> valueWithStops:@{
        @18: self.testColor,
    }];
}

+ (MGLStyleConstantValue<NSValue *> *)testEnum:(NSUInteger)value type:(const char *)type
{
    return [MGLStyleConstantValue<NSValue *> valueWithRawValue:[NSValue value:&value withObjCType:type]];
}

+ (MGLStyleFunction<NSValue *> *)testEnumFunction:(NSUInteger)value type:(const char *)type
{
    return [MGLStyleFunction<NSValue *> valueWithStops:@{
        @18: [self testEnum:value type:type],
    }];
}

@end
