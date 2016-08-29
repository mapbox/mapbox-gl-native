#import "MGLRuntimeStylingHelper.h"

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
    #import <UIKit/UIKit.h>
    #define MGLEdgeInsets UIEdgeInsets
#else
    #import <Cocoa/Cocoa.h>
    #define MGLEdgeInsets NSEdgeInsets
#endif

@implementation MGLRuntimeStylingHelper

+ (NSValue *)testPadding
{
    MGLEdgeInsets insets = {
        .top = 1,
        .left = 1,
        .bottom = 1,
        .right = 1,
    };
    return [NSValue value:&insets withObjCType:@encode(MGLEdgeInsets)];
}

+ (MGLStyleAttributeFunction *)testPaddingFunction
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    function.stops = @{
        @(18): self.testPadding,
    };
    return function;
}

+ (NSValue *)testOffset
{
    CGVector vector = CGVectorMake(1, 1);
    return [NSValue value:&vector withObjCType:@encode(CGVector)];
}

+ (MGLStyleAttributeFunction *)testOffsetFunction
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    function.stops = @{
        @(18): self.testOffset,
    };
    return function;
}

+ (NSArray *)testFont
{
    return @[@"Open Sans Regular", @"Arial Unicode MS Regular"];
}

+ (MGLStyleAttributeFunction *)testFontFunction
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    function.stops = @{
        @(18): self.testFont,
    };
    return function;
}

+ (NSArray *)testDashArray
{
    return @[@1, @2];
}

+ (MGLStyleAttributeFunction *)testDashArrayFunction
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    function.stops = @{
        @(18): self.testDashArray,
    };
    return function;
}

+ (NSNumber *)testNumber
{
    return @1;
}

+ (MGLStyleAttributeFunction *)testNumberFunction
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    function.stops = @{
        @(18): self.testNumber,
    };
    return function;
}

+ (NSNumber *)testBool
{
    return @YES;
}

+ (MGLStyleAttributeFunction *)testBoolFunction
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    function.stops = @{
        @(18): self.testBool,
    };
    return function;
}

+ (NSString *)testString
{
    return @"test";
}

+ (MGLStyleAttributeFunction *)testStringFunction
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    function.stops = @{
        @(18): self.testString,
    };
    return function;
}

+ (MGLColor *)testColor
{
    return [MGLColor redColor];
}

+ (MGLStyleAttributeFunction *)testColorFunction
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    function.stops = @{
        @(18): self.testColor,
    };
    return function;
}

+ (NSValue *)testEnum:(NSUInteger)value type:(const char *)type
{
    return [NSValue value:&value withObjCType:type];
}

+ (MGLStyleAttributeFunction *)testEnumFunction:(NSUInteger)value type:(const char *)type
{
    MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init];
    function.stops = @{
        @(18): [self testEnum:value type:type],
    };
    return function;
}

@end
