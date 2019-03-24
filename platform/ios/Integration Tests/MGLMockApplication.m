#import "MGLMockApplication.h"

#define TRACE() NSLog(@"%s", __PRETTY_FUNCTION__)

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
}

- (void)enterForeground {
    TRACE();
}

- (BOOL)openURL:(NSURL*)url {
    TRACE();
    return NO;
}

@end
