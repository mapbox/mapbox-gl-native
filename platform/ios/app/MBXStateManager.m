#import "MBXStateManager.h"
#import "MBXState.h"
#import "MBXViewController.h"

static NSString * const MBXStateManagerMapStateKey = @"mapStateKey";

@interface MBXStateManager()

@property (strong, nonatomic) MBXState *currentState;

@end

@implementation MBXStateManager

+ (instancetype) sharedManager {
    static dispatch_once_t once;
    static MBXStateManager* sharedManager;
    dispatch_once(&once, ^{
        sharedManager = [[self alloc] init];
    });

    return sharedManager;
}

- (MBXState*)currentState {
    NSData *encodedMapState = [[NSUserDefaults standardUserDefaults] objectForKey:MBXStateManagerMapStateKey];
    MBXState *currentState = (MBXState *)[NSKeyedUnarchiver unarchiveObjectWithData:encodedMapState];

    return currentState;
}

- (void)saveState:(MBXState*)mapState {
    NSData *encodedMapState = [NSKeyedArchiver archivedDataWithRootObject:mapState];
    [[NSUserDefaults standardUserDefaults] setObject:encodedMapState forKey:MBXStateManagerMapStateKey];
}

- (void)resetState {
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:MBXStateManagerMapStateKey];
}



@end
