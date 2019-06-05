#import "MGLAccountManager_Private.h"
#import "NSBundle+MGLAdditions.h"

#if TARGET_OS_OSX
#import "NSProcessInfo+MGLAdditions.h"
#endif

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
#import "MGLMapboxEvents.h"
#import "MBXSKUToken.h"

static NSString * const MGLAccountManagerExternalClassName = @"MBXAccounts";
static NSString * const MGLAccountManagerExternalMethodName = @"skuToken";
#endif

@interface MGLAccountManager ()

@property (atomic) NSString *accessToken;
@property (nonatomic) NSURL *apiBaseURL;

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
@property BOOL useExternalAccountManager;
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
    // Check if we should use an external accounts library (e.g., provided by navigation)
    id externalAccountsClass = NSClassFromString(MGLAccountManagerExternalClassName);
    SEL externalSKUTokenMethod = NSSelectorFromString(MGLAccountManagerExternalMethodName);
    if (externalAccountsClass != nil && [externalAccountsClass respondsToSelector:externalSKUTokenMethod]) {
        MGLAccountManager.sharedManager.useExternalAccountManager = YES;
    }
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

+ (NSString *)skuToken {
    if (MGLAccountManager.sharedManager.useExternalAccountManager) {
        id externalAccountsClass = NSClassFromString(MGLAccountManagerExternalClassName);
        SEL externalSKUTokenMethod = NSSelectorFromString(MGLAccountManagerExternalMethodName);
        return ((NSString *(*)(id, SEL))[externalAccountsClass methodForSelector:externalSKUTokenMethod])(externalAccountsClass, externalSKUTokenMethod);
    } else {
        return MBXSKUToken.skuToken;
    }
}

#endif

@end
