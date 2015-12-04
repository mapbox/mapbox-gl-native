#import "NSBundle+MGLAdditions.h"

#import "MGLAccountManager.h"

void mgl_linkBundleCategory() {}

@implementation NSBundle (MGLAdditions)

+ (instancetype)mgl_frameworkBundle
{
    return [self bundleForClass:[MGLAccountManager class]];
}

@end
