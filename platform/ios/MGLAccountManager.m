#import <Foundation/Foundation.h>

#import "MGLAccountManager.h"
#import "MGLMapboxEvents_Private.h"
#import "NSProcessInfo+MGLAdditions.h"

@interface MGLAccountManager()

@property (atomic) BOOL showsOptOutInApp;
@property (atomic) NSString *accessToken;

@end


@implementation MGLAccountManager

static MGLAccountManager *_sharedManager;

// Can be called from any thread.
//
+ (instancetype) sharedManager {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        if ( ! NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent) {
            void (^setupBlock)() = ^{
                _sharedManager = [[self alloc] init];
                _sharedManager.showsOptOutInApp = NO;
            };
            if ( ! [[NSThread currentThread] isMainThread]) {
                dispatch_sync(dispatch_get_main_queue(), ^{
                    setupBlock();
                });
            }
            else {
                setupBlock();
            }
        }
    });
    return _sharedManager;
}

+ (void) setMapboxMetricsEnabledSettingShownInApp:(BOOL)showsOptOut {
    [MGLAccountManager sharedManager].showsOptOutInApp = showsOptOut;
}

+ (BOOL) mapboxMetricsEnabledSettingShownInApp {
    return [MGLAccountManager sharedManager].showsOptOutInApp;
}

+ (void) setAccessToken:(NSString *) accessToken {
    [[MGLAccountManager sharedManager] setAccessToken:accessToken];

    // Update MGLMapboxEvents
    // NOTE: This is (likely) the initial setup of MGLMapboxEvents
    [MGLMapboxEvents sharedManager];
}

+ (NSString *) accessToken {
    return [MGLAccountManager sharedManager].accessToken;
}


@end