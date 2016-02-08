#import "NSBundle+MGLAdditions.h"

#import "MGLAccountManager.h"

void mgl_linkBundleCategory() {}

@implementation NSBundle (MGLAdditions)

+ (instancetype)mgl_frameworkBundle
{
    NSBundle *bundle = [self bundleForClass:[MGLAccountManager class]];
    if (![bundle.infoDictionary[@"CFBundlePackageType"] isEqualToString:@"FMWK"]) {
        // For static frameworks, the bundle is the containing application
        // bundle but the resources are still in the framework bundle.
        bundle = [NSBundle bundleWithPath:[bundle.privateFrameworksPath
                                           stringByAppendingPathComponent:@"Mapbox.framework"]];
    }
    return bundle;
}

@end
