#import "MGLAccountManager_Private.h"
#import "NSBundle+MGLAdditions.h"

#if TARGET_OS_OSX
#import "NSProcessInfo+MGLAdditions.h"
#endif

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
#import "MGLMapboxEvents.h"
#import "MBXSKUToken.h"
#endif

static const NSTimeInterval MGLAccountManagerSKUTokenLifespan = 3600;

@interface MGLAccountManager ()

@property (atomic) NSString *accessToken;
@property (nonatomic) NSURL *apiBaseURL;

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
@property (atomic) NSString *skuToken;
@property (atomic) NSDate *skuTokenExpiration;
#endif
@end

@implementation MGLAccountManager

#pragma mark - Internal

+ (void)load {
    // Read the initial configuration from Info.plist.
    NSString *accessToken = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"MGLMapboxAccessToken"];
    if (accessToken.length) {
        self.accessToken = accessToken;
    }

    NSString *apiBaseURL = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"MGLMapboxAPIBaseURL"];
    
    // If apiBaseURL is not a valid URL, [NSURL URLWithString:] will be `nil`.
    if (apiBaseURL.length && [NSURL URLWithString:apiBaseURL]) {
        [self setAPIBaseURL:[NSURL URLWithString:apiBaseURL]];
    }

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
    self.skuToken = [MBXSKUToken tokenForSKUID:MBXAccountsSKUIDMaps type:MBXAccountsSKUTypeUser];
#endif
}

+ (instancetype)sharedManager {
#if TARGET_OS_OSX
    if (NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent) {
        return nil;
    }
#endif
    
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

+ (void)setAccessToken:(NSString *)accessToken {
    accessToken = [accessToken stringByTrimmingCharactersInSet:
                   [NSCharacterSet whitespaceAndNewlineCharacterSet]];
    if (!accessToken.length) {
        return;
    }

    [MGLAccountManager sharedManager].accessToken = accessToken;

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
    dispatch_async(dispatch_get_main_queue(), ^{
        [MGLMapboxEvents setupWithAccessToken:accessToken];
    });
#endif
}

+ (NSString *)accessToken {
    return [MGLAccountManager sharedManager].accessToken;
}

+ (void)setAPIBaseURL:(NSURL *)apiBaseURL {
    [MGLAccountManager sharedManager].apiBaseURL = apiBaseURL;
}

+ (NSURL *)apiBaseURL {
    return [MGLAccountManager sharedManager].apiBaseURL;
}

#pragma mark - SKU Tokens

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR

+ (void)setSkuToken:(NSString *)skuToken {
    MGLAccountManager.sharedManager.skuTokenExpiration = [NSDate dateWithTimeIntervalSinceNow:MGLAccountManagerSKUTokenLifespan];
    MGLAccountManager.sharedManager.skuToken = skuToken;
}

+ (NSString *)skuToken {
    return [MGLAccountManager.sharedManager isSKUTokenExpired] ?
        [MBXSKUToken tokenForSKUID:MBXAccountsSKUIDMaps type:MBXAccountsSKUTypeUser] :
        MGLAccountManager.sharedManager.skuToken;
}

- (BOOL)isSKUTokenExpired {
    NSTimeInterval secondsUntilExpiration = [MGLAccountManager.sharedManager.skuTokenExpiration timeIntervalSinceNow];
    return secondsUntilExpiration < 0;
}

#endif

@end
