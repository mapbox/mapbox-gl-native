#import <Foundation/Foundation.h>

#import "MapboxGL.h"
#import "NSProcessInfo+MGLAdditions.h"

@interface MapboxGL()

@property (atomic) NSString *accessToken;

@end


@implementation MapboxGL

static MapboxGL *_sharedManager;

// Can be called from any thread. Called implicitly from any
// public class convenience methods.
//
+ (id) sharedInstanceWithAccessToken:(NSString *)token {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        if ( ! NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent) {
            void (^setupBlock)() = ^{
                _sharedManager = [[self alloc] init];
                _sharedManager.accessToken = token;
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