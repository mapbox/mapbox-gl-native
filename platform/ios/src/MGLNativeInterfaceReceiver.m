#import "MGLNativeInterfaceReceiver.h"

#import "MGLLoggingConfiguration_Private.h"
#import "MGLNetworkConfiguration_Private.h"

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
#import "MGLAccountManager_Private.h"
#endif

@implementation MGLNativeInterfaceReceiver

static MGLNativeInterfaceReceiver *instance = nil;

+ (MGLNativeInterfaceReceiver *)shared {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[MGLNativeInterfaceReceiver alloc] init];
    });
    return instance;
}

#pragma mark - MGLNativeAppleInterfaceManager delegate -

- (NSURLSessionConfiguration *)nai_sessionConfiguration {
    return [MGLNetworkConfiguration sharedManager].sessionConfiguration;
}

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
- (NSString *)nai_accountTypeKey {
    return MGLMapboxAccountTypeKey;
}

- (NSString *)nai_skuToken {
    return MGLAccountManager.skuToken;
}
#endif

- (void)nai_startDownloadEvent:(NSString *)event type:(NSString *)type {
    [[MGLNetworkConfiguration sharedManager] startDownloadEvent:event type:@"tile"];
}

- (void)nai_cancelDownloadEventForResponse:(NSURLResponse *)response {
    [[MGLNetworkConfiguration sharedManager] cancelDownloadEventForResponse:response];
}

- (void)nai_stopDownloadEventForResponse:(NSURLResponse *)response {
    [[MGLNetworkConfiguration sharedManager] stopDownloadEventForResponse:response];
}

@end
