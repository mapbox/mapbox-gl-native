#import "NSBundle+MGLAdditions.h"

#import "MGLAccountManager.h"

@implementation NSBundle (MGLAdditions)

+ (instancetype)mgl_frameworkBundle {
    NSBundle *bundle = [self bundleForClass:[MGLAccountManager class]];

    if (![bundle.infoDictionary[@"CFBundlePackageType"] isEqualToString:@"FMWK"] && [bundle pathForResource:@"Mapbox" ofType:@"bundle"]) {
        // For static frameworks, the bundle is the containing application
        // bundle but the resources are in Mapbox.bundle.
        bundle = [self bundleWithPath:[bundle pathForResource:@"Mapbox" ofType:@"bundle"]];
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

@end
