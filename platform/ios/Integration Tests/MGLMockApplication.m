#import "MGLMockApplication.h"

#define TRACE() NSLog(@"%s", __PRETTY_FUNCTION__)

@interface MGLMockApplication ()
@property(nonatomic, readwrite) UIApplicationState applicationState;
@end

@implementation MGLMockApplication

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

- (BOOL)openURL:(NSURL*)url {
    TRACE();
    return NO;
}

@end
