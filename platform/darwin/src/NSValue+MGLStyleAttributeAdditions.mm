#import "NSValue+MGLStyleAttributeAdditions.h"

#import "NSValue+MGLStyleAttributeAdditions_Private.h"

#include <array>

#if TARGET_OS_IPHONE
    #import <UIKit/UIKit.h>
    #define MGLEdgeInsets UIEdgeInsets
#else
    #define MGLEdgeInsets NSEdgeInsets
#endif

@implementation NSValue (MGLStyleAttributeAdditions)

+ (instancetype)mgl_valueWithOffsetArray:(std::array<float, 2>)offsetArray
{
    CGVector vector = CGVectorMake(offsetArray[0], offsetArray[1]);
    return [NSValue value:&vector withObjCType:@encode(CGVector)];
}

+ (instancetype)mgl_valueWithPaddingArray:(std::array<float, 4>)paddingArray
{
    // Style specification defines padding in clockwise order: top, right, bottom, left.
    // Foundation defines padding in counterclockwise order: top, left, bottom, right.
    MGLEdgeInsets insets = {
        .top = paddingArray[0],
        .right = paddingArray[1],
        .bottom = paddingArray[2],
        .left = paddingArray[3],
    };
    return [NSValue value:&insets withObjCType:@encode(MGLEdgeInsets)];
}

- (BOOL)isFunction
{
    return NO;
}

- (mbgl::style::PropertyValue<std::array<float, 2>>)mbgl_offsetPropertyValue
{
    return { self.mgl_offsetArrayValue };
}

- (std::array<float, 2>)mgl_offsetArrayValue
{
    NSAssert(strcmp(self.objCType, @encode(CGVector)) == 0, @"Value does not represent a CGVector");
    CGVector vector;
    [self getValue:&vector];
    return {
        static_cast<float>(vector.dx),
        static_cast<float>(vector.dy),
    };
}

- (mbgl::style::PropertyValue<std::array<float, 4>>)mbgl_paddingPropertyValue
{
    return { self.mgl_paddingArrayValue };
}

- (std::array<float, 4>)mgl_paddingArrayValue
{
    NSAssert(strcmp(self.objCType, @encode(MGLEdgeInsets)) == 0, @"Value does not represent an NSEdgeInsets/UIEdgeInsets");
    MGLEdgeInsets insets;
    [self getValue:&insets];
    // Style specification defines padding in clockwise order: top, right, bottom, left.
    return {
        static_cast<float>(insets.top),
        static_cast<float>(insets.right),
        static_cast<float>(insets.bottom),
        static_cast<float>(insets.left),
    };
}

@end
