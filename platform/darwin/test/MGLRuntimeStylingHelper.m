#import "MGLRuntimeStylingHelper.h"

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
#import <UIKit/UIKit.h>
#else
#import <Cocoa/Cocoa.h>
#endif

@implementation MGLRuntimeStylingHelper

+ (NSArray *)testPadding
{
    return @[@1.0f, @1.0f, @1.0f, @1.0f];
}

+ (NSArray *)testOffset
{
    return @[@1.0f, @1.0f];
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

@end
