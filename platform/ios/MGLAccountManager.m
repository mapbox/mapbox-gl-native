#import <Foundation/Foundation.h>

#import "MGLAccountManager_Private.h"
#import "MGLMapboxEvents.h"
#import "NSProcessInfo+MGLAdditions.h"

@interface MGLAccountManager()

@property (atomic) BOOL mapboxMetricsEnabledSettingShownInApp;
@property (atomic) NSString *accessToken;

@end


@implementation MGLAccountManager

+ (void)load {
    // Read initial configuration from Info.plist.
    NSBundle *bundle = [NSBundle bundleForClass:self];
    self.accessToken = [bundle objectForInfoDictionaryKey:@"MGLMapboxAccessToken"];
    NSNumber *shownInAppNumber = [bundle objectForInfoDictionaryKey:@"MGLMapboxMetricsEnabledSettingShownInApp"];
    if (shownInAppNumber) {
        [MGLAccountManager sharedManager].mapboxMetricsEnabledSettingShownInApp = [shownInAppNumber boolValue];
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

+ (void) setMapboxMetricsEnabledSettingShownInApp:(BOOL)shown {
    [MGLAccountManager sharedManager].mapboxMetricsEnabledSettingShownInApp = shown;
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