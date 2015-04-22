#import <Foundation/Foundation.h>

#import "MapboxGL.h"

@interface MapboxGL()

@end

@property (atomic) NSString *accessToken;

@implementation MapboxGL


// Can be called from any thread. Called implicitly from any
// public class convenience methods.
//
+ (instancetype) sharedInstanceWithAccessToken:(NSString *)token {
    static dispatch_once_t onceToken;
    static MapboxGL *_sharedManager;
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

@end