#import "NSBundle+MGLAdditions.h"

#import "MGLAccountManager.h"

@implementation NSBundle (MGLAdditions)

+ (instancetype)mgl_frameworkBundle {
    NSBundle *bundle = [self bundleForClass:[MGLAccountManager class]];
    if (![bundle.infoDictionary[@"CFBundlePackageType"] isEqualToString:@"FMWK"] && !bundle.mgl_resourcesDirectory) {
        // For static frameworks, the bundle is the containing application
        // bundle but the resources are still in the framework bundle.
        bundle = [NSBundle bundleWithPath:[bundle.privateFrameworksPath
                                           stringByAppendingPathComponent:@"Mapbox.framework"]];
    }
    return bundle;
}

+ (nullable NSString *)mgl_frameworkBundleIdentifier {
    return self.mgl_frameworkInfoDictionary[@"CFBundleIdentifier"];
}

+ (nullable NS_DICTIONARY_OF(NSString *, id) *)mgl_frameworkInfoDictionary {
    NSBundle *bundle = self.mgl_frameworkBundle;
    if (bundle.mgl_resourcesDirectory) {
        NSString *infoPlistPath = [bundle pathForResource:@"Info"
                                                   ofType:@"plist"
                                              inDirectory:bundle.mgl_resourcesDirectory];
        return [NSDictionary dictionaryWithContentsOfFile:infoPlistPath];
    } else {
        return bundle.infoDictionary;
    }
}

- (NSString *)mgl_resourcesDirectory {
    return [self pathForResource:@"Mapbox" ofType:@"bundle"] ? @"Mapbox.bundle" : nil;
}

@end
