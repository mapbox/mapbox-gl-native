#import "NSBundle+MGLAdditions.h"

#import "MGLAccountManager.h"

void mgl_linkBundleCategory() {}

@implementation NSBundle (MGLAdditions)

+ (instancetype)mgl_resourceBundle {
    return [self bundleWithPath:[self mgl_resourceBundlePath]];
}

+ (NSString *)mgl_resourceBundlePath {
    NSString *resourceBundlePath = [[self bundleForClass:[MGLAccountManager class]]
                                    pathForResource:@"Mapbox" ofType:@"bundle"];
    if (!resourceBundlePath) {
        resourceBundlePath = [[self mainBundle] bundlePath];
    }
    return resourceBundlePath;
}

@end
