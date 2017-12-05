#import "MGLSDKUpdateChecker.h"
#import "NSBundle+MGLAdditions.h"
#import "NSProcessInfo+MGLAdditions.h"

@implementation MGLSDKUpdateChecker

+ (void)checkForUpdates {
#if TARGET_IPHONE_SIMULATOR
    // Abort if running in a playground.
    if ([[NSBundle mainBundle].bundleIdentifier hasPrefix:@"com.apple.dt.playground."] ||
        NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent) {
        return;
    }

    NSString *currentVersion = [NSBundle mgl_frameworkInfoDictionary][@"MGLSemanticVersionString"];

    // Skip version check if weʼre doing gl-native development, as the framework
    // version is `1` until built for packaging.
    if ([currentVersion isEqualToString:@"1.0.0"]) {
        return;
    }

    NSURL *url = [NSURL URLWithString:@"https://www.mapbox.com/ios-sdk/latest_version"];
    [[NSURLSession.sharedSession dataTaskWithURL:url completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        if (error || ((NSHTTPURLResponse *)response).statusCode != 200) {
            return;
        }

        NSString *latestVersion = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        latestVersion = [latestVersion stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
        if (![currentVersion isEqualToString:latestVersion]) {
            NSString *updateAvailable = [NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"SDK_UPDATE_AVAILABLE", nil, nil, @"Mapbox Maps SDK for iOS version %@ is now available:", @"Developer-only SDK update notification; {latest version, in format x.x.x}"), latestVersion];
            NSLog(@"%@ https://github.com/mapbox/mapbox-gl-native/releases/tag/ios-v%@", updateAvailable, latestVersion);
        }
    }] resume];
#endif
}

@end
