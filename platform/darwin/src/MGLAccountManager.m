#import "MGLAccountManager_Private.h"
#import "NSBundle+MGLAdditions.h"
#import "NSProcessInfo+MGLAdditions.h"

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
#import "MGLMapboxEvents.h"

#import "FABKitProtocol.h"
#import "Fabric+FABKits.h"

@interface MGLAccountManager () <FABKit>

@property (atomic) NSString *accessToken;

@end
#else
@interface MGLAccountManager ()

@property (atomic) NSString *accessToken;

@end
#endif

@implementation MGLAccountManager

#pragma mark - Internal

+ (void)load {
    // Read the initial configuration from Info.plist.
    NSString *accessToken = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"MGLMapboxAccessToken"];
    if (accessToken.length) {
        self.accessToken = accessToken;
    }
}

+ (instancetype)sharedManager {
    if (NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent) {
        return nil;
    }
    static dispatch_once_t onceToken;
    static MGLAccountManager *_sharedManager;
    void (^setupBlock)(void) = ^{
        dispatch_once(&onceToken, ^{
            _sharedManager = [[self alloc] init];
        });
    };
    if (![[NSThread currentThread] isMainThread]) {
        dispatch_sync(dispatch_get_main_queue(), ^{
            setupBlock();
        });
    } else {
        setupBlock();
    }
    return _sharedManager;
}

+ (BOOL)mapboxMetricsEnabledSettingShownInApp {
    NSLog(@"mapboxMetricsEnabledSettingShownInApp is no longer necessary; the Mapbox Maps SDK for iOS has changed to always provide a telemetry setting in-app.");
    return YES;
}

+ (void)setAccessToken:(NSString *)accessToken {
    accessToken = [accessToken stringByTrimmingCharactersInSet:
                   [NSCharacterSet whitespaceAndNewlineCharacterSet]];
    if (!accessToken.length) {
        return;
    }

    [MGLAccountManager sharedManager].accessToken = accessToken;

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
    // Update MGLMapboxEvents
    // NOTE: This is (likely) the initial setup of MGLMapboxEvents
    [MGLMapboxEvents sharedManager];
#endif
}

+ (NSString *)accessToken {
    return [MGLAccountManager sharedManager].accessToken;
}

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR

#pragma mark - Fabric

+ (NSString *)bundleIdentifier {
    return [NSBundle mgl_frameworkBundleIdentifier];
}

+ (NSString *)kitDisplayVersion {
    return [NSBundle mgl_frameworkInfoDictionary][@"CFBundleShortVersionString"];
}

+ (void)initializeIfNeeded {
    Class fabric = NSClassFromString(@"Fabric");

    if (fabric) {
        NSDictionary *configuration = [fabric configurationDictionaryForKitClass:[MGLAccountManager class]];
        if (!configuration || !configuration[@"accessToken"]) {
            NSLog(@"Configuration dictionary returned by Fabric was nil or doesn’t have accessToken. Can’t initialize MGLAccountManager.");
            return;
        }
        [self setAccessToken:configuration[@"accessToken"]];
        MGLAccountManager *sharedAccountManager = [self sharedManager];
        NSLog(@"MGLAccountManager was initialized with access token: %@", sharedAccountManager.accessToken);
    } else {
        NSLog(@"MGLAccountManager is used in a project that doesn’t have Fabric.");
    }
}

#endif

@end
