#import <Foundation/Foundation.h>

#import "MGLAccountManager.h"
#import "MGLMapboxEvents.h"
#import "NSProcessInfo+MGLAdditions.h"

@interface MGLAccountManager()

@property (atomic) BOOL mapboxMetricsEnabledSettingShownInApp;
@property (atomic) NSString *accessToken;

@end


@implementation MGLAccountManager

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
            _sharedManager.mapboxMetricsEnabledSettingShownInApp = NO;
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
    [[MGLAccountManager sharedManager] setAccessToken:accessToken];

    // Update MGLMapboxEvents
    // NOTE: This is (likely) the initial setup of MGLMapboxEvents
    [MGLMapboxEvents sharedManager];

    // Count App Users
    [MGLMapboxEvents pushEvent:MGLEventTypeAppUserCount withAttributes:nil];
}

+ (NSString *) accessToken {
    return [MGLAccountManager sharedManager].accessToken;
}


@end