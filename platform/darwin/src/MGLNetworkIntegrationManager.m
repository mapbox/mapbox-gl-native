#import "MGLNetworkIntegrationManager.h"

#import "MGLLoggingConfiguration_Private.h"
#import "MGLNetworkConfiguration_Private.h"

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
#import "MGLAccountManager_Private.h"
#endif

@implementation MGLNetworkIntegrationManager

static MGLNetworkIntegrationManager *instance = nil;

+ (MGLNetworkIntegrationManager *)shared {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[MGLNetworkIntegrationManager alloc] init];
    });
    return instance;
}

#pragma mark - MGLNativeAppleInterfaceManager delegate -

- (NSURLSessionConfiguration *)sessionConfiguration {
    return [MGLNetworkConfiguration sharedManager].sessionConfiguration;
}

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
- (NSString *)accountTypeKey {
    return MGLMapboxAccountTypeKey;
}

- (NSString *)skuToken {
    return MGLAccountManager.skuToken;
}
#endif

- (void)startDownloadEvent:(NSString *)event type:(NSString *)type {
    [[MGLNetworkConfiguration sharedManager] startDownloadEvent:event type:@"tile"];
}

- (void)cancelDownloadEventForResponse:(NSURLResponse *)response {
    [[MGLNetworkConfiguration sharedManager] cancelDownloadEventForResponse:response];
}

- (void)stopDownloadEventForResponse:(NSURLResponse *)response {
    [[MGLNetworkConfiguration sharedManager] stopDownloadEventForResponse:response];
}

@end
