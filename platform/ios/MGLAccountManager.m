#import <Foundation/Foundation.h>

#import "MGLAccountManager.h"
#import "NSProcessInfo+MGLAdditions.h"
#import "MGLMapboxEvents.h"

@interface MGLAccountManager()

@property (atomic) NSString *accessToken;

@end


@implementation MGLAccountManager

static MGLAccountManager *_sharedManager;

// Can be called from any thread. Called implicitly from any
// public class convenience methods.
//
+ (instancetype) sharedInstanceWithAccessToken:(NSString *)token {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        if ( ! NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent) {
            void (^setupBlock)() = ^{
                _sharedManager = [[self alloc] init];
                _sharedManager.accessToken = token;
                [MGLMapboxEvents setToken:token];
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

+ (NSString *) getAccessToken {
    if (_sharedManager) {
        return _sharedManager.accessToken;
    }
    return nil;
}

@end