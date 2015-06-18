#import "MGLAccountManager_Private.h"
#import "MGLMapboxEvents.h"
#import "NSProcessInfo+MGLAdditions.h"

@interface MGLAccountManager()

@property (atomic) BOOL mapboxMetricsEnabledSettingShownInApp;
@property (atomic) NSString *accessToken;

@end


@implementation MGLAccountManager

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


@end