#import "MGLMockApplication.h"

#define TRACE() NSLog(@"%s", __PRETTY_FUNCTION__)

@interface MGLMockApplication ()
@property(nonatomic, readwrite) UIApplicationState applicationState;
@end

@implementation MGLMockApplication

- (void)dealloc {

    if (_applicationState != UIApplicationStateActive) {
        [self enterForeground];
    }
    
    if (_delegate) {
        CFRelease((CFTypeRef)_delegate);
    }
}

- (instancetype)init {
    if ((self = [super init])) {
        _applicationState = UIApplicationStateActive;
        _statusBarOrientation = UIInterfaceOrientationUnknown;
    }
    return self;
}

- (void)enterBackground {
    TRACE();
    self.applicationState = UIApplicationStateInactive;
    [[NSNotificationCenter defaultCenter] postNotificationName:UIApplicationWillResignActiveNotification object:self];
    [[NSNotificationCenter defaultCenter] postNotificationName:UIApplicationDidEnterBackgroundNotification object:self];
    self.applicationState = UIApplicationStateBackground;
}

- (void)enterForeground {
    TRACE();
    self.applicationState = UIApplicationStateInactive;
    [[NSNotificationCenter defaultCenter] postNotificationName:UIApplicationWillEnterForegroundNotification object:self];
    [[NSNotificationCenter defaultCenter] postNotificationName:UIApplicationDidBecomeActiveNotification object:self];
    self.applicationState = UIApplicationStateActive;
}

- (void)setDelegate:(id<UIApplicationDelegate>)delegate {
    // Property is `assign`, but we want to retain
    if (_delegate != delegate) {
        if (_delegate) {
            CFRelease((CFTypeRef)_delegate);
        }
        
        _delegate = delegate;

        if (_delegate) {
            CFRetain((CFTypeRef)_delegate);
        }
    }
}

- (BOOL)openURL:(NSURL*)url {
    TRACE();
    return NO;
}

@end
