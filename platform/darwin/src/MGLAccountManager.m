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

NSString * const MGLMapboxAccountTypeKey = @"MGLMapboxAccountType";

@interface MGLAccountManager ()

@property (atomic) NSString *accessToken;
@property (nonatomic) NSURL *apiBaseURL;

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
    Class mbx = NSClassFromString(MGLAccountManagerExternalClassName);
    
    if ([mbx respondsToSelector:NSSelectorFromString(MGLAccountManagerExternalMethodName)]) {
        return (NSString *)[mbx valueForKeyPath:MGLAccountManagerExternalMethodName];
    }
    
    return MBXSKUToken.skuToken;
}

#endif

@end
