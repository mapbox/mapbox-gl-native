#import "MBXStateManager.h"
#import <Mapbox/Mapbox.h>

@implementation MBXStateManager

+ (instancetype) sharedManager {
    static dispatch_once_t once;
    static MBXStateManager* sharedManager;
    dispatch_once(&once, ^{
        sharedManager = [[self alloc] init];
        sharedManager.currentState = [[MBXState alloc] init];
    });

    return sharedManager;
}

- (MBXState*)currentState {
    return _currentState;
}

- (void)saveState:(MBXState*)state forKey:(NSObject*)key {

}

-(void)restoreState {
    // Load from NSUserDefaults
}

-(void)resetState {
    self.currentState = nil;
}



@end
