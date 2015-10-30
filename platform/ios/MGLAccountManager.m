#import "MGLAccountManager_Private.h"
#import "MGLMapboxEvents.h"
#import "NSProcessInfo+MGLAdditions.h"

#import <Fabric/FABKitProtocol.h>
#import <Fabric/Fabric+FABKits.h>

@interface MGLAccountManager() <FABKit>

@property (atomic) BOOL mapboxMetricsEnabledSettingShownInApp;
@property (atomic) NSString *accessToken;

@end

@implementation MGLAccountManager

#pragma mark - Internal

+ (void)load {
    // Read the initial configuration from Info.plist. The shown-in-app setting
    // preempts the Settings bundle check in -[MGLMapboxEvents init] triggered
    // by setting the access token.
    NSBundle *bundle = [NSBundle mainBundle];
    NSNumber *shownInAppNumber = [bundle objectForInfoDictionaryKey:@"MGLMapboxMetricsEnabledSettingShownInApp"];
    if (shownInAppNumber) {
        [MGLAccountManager sharedManager].mapboxMetricsEnabledSettingShownInApp = [shownInAppNumber boolValue];
    }
    NSString *accessToken = [bundle objectForInfoDictionaryKey:@"MGLMapboxAccessToken"];
    if (accessToken.length) {
        self.accessToken = accessToken;
    }
}

// Can be called from any thread.
//
+ (instancetype) sharedManager {
    if (NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent) {
        return nil;
    }
    static dispatch_once_t onceToken;
    static MGLAccountManager *_sharedManager;
    void (^setupBlock)() = ^{
        dispatch_once(&onceToken, ^{
            _sharedManager = [[self alloc] init];
        });
    };
    if ( ! [[NSThread currentThread] isMainThread]) {
        dispatch_sync(dispatch_get_main_queue(), ^{
            setupBlock();
        });
    }
    else {
        setupBlock();
    }
    return _sharedManager;
}

+ (BOOL) mapboxMetricsEnabledSettingShownInApp {
    return [MGLAccountManager sharedManager].mapboxMetricsEnabledSettingShownInApp;
}

+ (void) setAccessToken:(NSString *) accessToken {
    if ( ! [accessToken length]) return;
    
    [MGLAccountManager sharedManager].accessToken = accessToken;

    // Update MGLMapboxEvents
    // NOTE: This is (likely) the initial setup of MGLMapboxEvents
    [MGLMapboxEvents sharedManager];
}

+ (NSString *) accessToken {
    return [MGLAccountManager sharedManager].accessToken;
}

#pragma mark - Fabric

+ (NSString *)bundleIdentifier {
    return @"com.mapbox.sdk.ios";
}

+ (NSString *)kitDisplayVersion {
    return @"3.0.0-pre.3";
}

+ (void)initializeIfNeeded {
    Class fabric = NSClassFromString(@"Fabric");

    if (fabric) {
        NSDictionary *configuration = [fabric configurationDictionaryForKitClass:[MGLAccountManager class]];
        if ( ! configuration || ! configuration[@"accessToken"]) {
            NSLog(@"Configuration dictionary returned by Fabric was nil or doesn't have accessToken. Can't initialize MGLAccountManager.");
            return;
        }
        [self setAccessToken:configuration[@"accessToken"]];
        MGLAccountManager *sharedAccountManager = [self sharedManager];
        NSLog(@"MGLAccountManager was initialized with access token: %@", sharedAccountManager.accessToken);
    } else {
        NSLog(@"MGLAccountManager is used in a project that doesn't have Fabric.");
    }
}

@end
