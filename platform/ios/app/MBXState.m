#import "MBXState.h"

NSString *const MBXCamera = @"MBXCamera";
NSString *const MBXUserTrackingMode = @"MBXUserTrackingMode";
NSString *const MBXShowsUserLocation = @"MBXShowsUserLocation";
NSString *const MBXDebugMask = @"MBXDebugMask";
NSString *const MBXShowsZoomLevelHUD =  @"MBXShowsZoomLevelHUD";
NSString *const MBXShowsTimeFrameGraph = @"MBXShowsFrameTimeGraph";


@implementation MBXState

- (instancetype) init {
    if ((self = super.self)) {
        NSMutableDictionary *dictionary = [NSMutableDictionary dictionaryWithObjectsAndKeys: @"", MBXCamera,
            @"", MBXUserTrackingMode,
            @"", MBXShowsUserLocation,
            @"", MBXDebugMask,
            @"", MBXShowsZoomLevelHUD,
            @"", MBXShowsTimeFrameGraph,
            nil];

        _state = dictionary;
    }
    return self;
}

- (void)saveMapCameraState:(MGLMapCamera *)camera {
    _camera = camera;
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSData *archivedCamera = [NSKeyedArchiver archivedDataWithRootObject:camera];
    [self.state setValue:camera forKey:@"MBXCamera"];
    [defaults setObject:archivedCamera forKey:@"MBXCamera"];
    [defaults synchronize];
}

- (void)saveUserTrackingModeState:(NSInteger)trackingMode; {
    _userTrackingMode = trackingMode;
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [self.state setValue:@(trackingMode) forKey:@"MBXUserTrackingMode"];
    [defaults setInteger:trackingMode forKey:@"MBXUserTrackingMode"];
    [defaults synchronize];
}

- (void)saveShowsUserLocationState:(BOOL)showUserLocation {
    _showsUserLocation = showUserLocation;
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [self.state setValue:@(showUserLocation) forKey:@"MBXShowsUserLocation"];
    [defaults setBool:showUserLocation forKey:@"MBXShowsUserLocation"];
    [defaults synchronize];
}

- (void)saveDebugMaskState:(NSInteger)debugMask {
    _debugMask = debugMask;
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [self.state setValue:@(debugMask) forKey:@"MBXDebugMask"];
    [defaults setInteger:debugMask forKey:@"MBXDebugMask"];
    [defaults synchronize];
}

- (void)saveZoomLevelHUDState:(BOOL)showsZoomLevelHUD {
    _showsZoomLevelHUD = showsZoomLevelHUD;
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [self.state setValue:@(showsZoomLevelHUD) forKey:@"MBXShowsZoomLevelHUD"];
    [defaults setBool:showsZoomLevelHUD forKey:@"MBXShowsZoomLevelHUD"];
    [defaults synchronize];
}

- (void)saveDisplayTimeFrameGraphState:(BOOL)displayTimeFramGraphState {
    _showsTimeFrameGraph = displayTimeFramGraphState;
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [self.state setValue:@(displayTimeFramGraphState) forKey:@"MBXShowsFrameTimeGraph"];
    [defaults setBool:displayTimeFramGraphState forKey:@"MBXShowsFrameTimeGraph"];
    [defaults synchronize];
}

@end
