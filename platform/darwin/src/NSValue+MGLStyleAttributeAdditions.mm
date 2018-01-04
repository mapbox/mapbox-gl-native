#import "NSValue+MGLStyleAttributeAdditions.h"
#import "MGLLight.h"
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
#if !TARGET_OS_IPHONE
    // Style specification assumes an origin at the upper-left corner.
    // macOS defines an origin at the lower-left corner.
    vector.dy *= -1;
#endif
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

- (std::array<float, 2>)mgl_offsetArrayValue
{
    NSAssert(strcmp(self.objCType, @encode(CGVector)) == 0, @"Value does not represent a CGVector");
    CGVector vector;
    [self getValue:&vector];
#if !TARGET_OS_IPHONE
    vector.dy *= -1;
#endif
    return {
        static_cast<float>(vector.dx),
        static_cast<float>(vector.dy),
    };
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

- (std::array<float, 3>)mgl_lightPositionArrayValue
{
    NSAssert(strcmp(self.objCType, @encode(MGLSphericalPosition)) == 0, @"Value does not represent an MGLSphericalPosition");
    MGLSphericalPosition lightPosition;
    [self getValue:&lightPosition];
    // Style specification defines padding in clockwise order: top, right, bottom, left.
    return {
        static_cast<float>(lightPosition.radial),
        static_cast<float>(lightPosition.azimuthal),
        static_cast<float>(lightPosition.polar),
    };
}

@end
