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

+ (NSValue *)testOffset
{
    CGVector vector = CGVectorMake(1, 1);
    return [NSValue value:&vector withObjCType:@encode(CGVector)];
}

+ (NSArray *)testFont
{
    return @[@"Open Sans Regular", @"Arial Unicode MS Regular"];
}

+ (NSArray *)testDashArray
{
    return @[@1, @2];
}

+ (NSNumber *)testNumber
{
    return @1;
}

+ (NSNumber *)testBool
{
    return @YES;
}

+ (NSString *)testString
{
    return @"test";
}

+ (MGLColor *)testColor
{
    return [MGLColor redColor];
}

+ (NSValue *)testEnum:(NSUInteger)value type:(const char *)type
{
    return [NSValue value:&value withObjCType:type];
}

@end
