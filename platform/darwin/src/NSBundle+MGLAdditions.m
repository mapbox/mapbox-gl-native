#import "NSBundle+MGLAdditions.h"

#import "MGLAccountManager.h"

@implementation NSBundle (MGLAdditions)

+ (instancetype)mgl_frameworkBundle {
    NSBundle *bundle = [self bundleForClass:[MGLAccountManager class]];

    if (![bundle.infoDictionary[@"CFBundlePackageType"] isEqualToString:@"FMWK"]) {
        // For static frameworks, the bundle is the containing application
        // bundle but the resources are in Mapbox.bundle.
        NSString *bundlePath = [bundle pathForResource:@"Mapbox" ofType:@"bundle"];
        if (bundlePath) {
            bundle = [self bundleWithPath:bundlePath];
        } else {
            [NSException raise:@"MGLBundleNotFoundException" format:
             @"The Mapbox framework bundle could not be found. If using the Mapbox Maps SDK for iOS as a static framework, make sure that Mapbox.bundle is copied into the root of the app bundle."];
        }
    }

    return bundle;
}

+ (nullable NSString *)mgl_frameworkBundleIdentifier {
    return self.mgl_frameworkInfoDictionary[@"CFBundleIdentifier"];
}

+ (nullable NS_DICTIONARY_OF(NSString *, id) *)mgl_frameworkInfoDictionary {
    NSBundle *bundle = self.mgl_frameworkBundle;
    return bundle.infoDictionary;
}

+ (nullable NSString *)mgl_applicationBundleIdentifier {
    NSString *bundleIdentifier = [NSBundle mainBundle].bundleIdentifier;
    if (!bundleIdentifier) {
        // There’s no main bundle identifier when running in a unit test bundle.
        bundleIdentifier = [NSBundle bundleForClass:[MGLAccountManager class]].bundleIdentifier;
    }
    return bundleIdentifier;
}

@end
