#import "MBXStateManager.h"
#import <Mapbox/Mapbox.h>
#import "MBXState.h"

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
    
    if (!_currentState) {
        _currentState = [[MBXState alloc] init];
        
        NSMutableDictionary *dictionary = [[NSUserDefaults standardUserDefaults] objectForKey:@"mapStateKey"];
        if (dictionary[MBXCamera]) {
            _currentState.camera = dictionary[MBXCamera];
        }
        
        if (dictionary[MBXUserTrackingMode]) {
            _currentState.userTrackingMode = [dictionary[MBXUserTrackingMode] boolValue];
        }
        
        if (dictionary[MBXShowsUserLocation]) {
            _currentState.showsUserLocation = [dictionary[MBXShowsUserLocation] boolValue];
        }
        
        if (dictionary[MBXDebugMaskValue]) {
            _currentState.debugMask = ((NSNumber *)dictionary[MBXDebugMaskValue]).intValue;
        }

        if (dictionary[MBXShowsZoomLevelOrnament]) {
            _currentState.showsZoomLevelOrnament = [dictionary[MBXShowsZoomLevelOrnament] boolValue];
        }
        
        if (dictionary[MBXShowsTimeFrameGraph]) {
            _currentState.showsTimeFrameGraph = [dictionary[MBXShowsTimeFrameGraph] boolValue];
        }
        
    }
    return _currentState;
}

- (void)saveState:(MBXState*)state {
    _currentState = state;
    
    if (state) {
        NSMutableDictionary *dictionary = [NSMutableDictionary dictionary];
        if (state.camera) {
            [dictionary setObject:state.camera forKey:MBXCamera];
        }
        
        [dictionary setValue:@(state.userTrackingMode) forKey:MBXUserTrackingMode];
        [dictionary setValue:@(state.showsUserLocation) forKey:MBXShowsUserLocation];
        [dictionary setValue:@(state.debugMask) forKey:MBXDebugMaskValue];
        [dictionary setValue:@(state.showsZoomLevelOrnament) forKey:MBXShowsZoomLevelOrnament];
        [dictionary setValue:@(state.showsTimeFrameGraph) forKey:MBXShowsTimeFrameGraph];
        
        [[NSUserDefaults standardUserDefaults] setObject:dictionary forKey:@"mapStateKey"];

    } else {
        [self resetState];
    }

}

-(void)restoreState {
    
}

-(void)resetState {
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:@"mapStateKey"];
}



@end
