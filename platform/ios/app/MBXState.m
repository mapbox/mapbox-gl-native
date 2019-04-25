#import "MBXState.h"

NSString *const MBXCamera = @"MBXCamera";
NSString *const MBXUserTrackingMode = @"MBXUserTrackingMode";
NSString *const MBXShowsUserLocation = @"MBXShowsUserLocation";
NSString *const MBXDebugMask = @"MBXDebugMask";
NSString *const MBXShowsZoomLevelHUD =  @"MBXShowsZoomLevelHUD";
NSString *const MBXShowsTimeFrameGraph = @"MBXShowsFrameTimeGraph";

@implementation MBXState

- (instancetype) init {
    if (self = [super init]) {
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

- (void) setCamera:(MGLMapCamera *)camera {
    self.camera = camera;
}

- (MGLMapCamera*)camera {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSData *archivedCamera = [defaults objectForKey:@"MBXCamera"];
    MGLMapCamera *unarchivedCamera = archivedCamera ? [NSKeyedUnarchiver unarchiveObjectWithData:archivedCamera] : nil;
    return unarchivedCamera;
}

-(void)setTest:(NSObject *)test {
    _test = test;
}

- (void)setUserTrackingMode:(MGLUserTrackingMode)userTrackingMode {
     _userTrackingMode = userTrackingMode;
}

- (MGLUserTrackingMode)userTrackingMode {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    MGLUserTrackingMode uncheckedTrackingMode = [defaults integerForKey:@"MBXUserTrackingMode"];

    if (uncheckedTrackingMode >= 0 &&
        (MGLUserTrackingMode)uncheckedTrackingMode >= MGLUserTrackingModeNone &&
        (MGLUserTrackingMode)uncheckedTrackingMode <= MGLUserTrackingModeFollowWithCourse)
    {
        return uncheckedTrackingMode;
    }

    return 0;
}

- (void)setShowsUserLocation:(BOOL)showsUserLocation {
    self.showsUserLocation = showsUserLocation;
}

- (BOOL)showsUserLocation {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSInteger showsUserLocation = [defaults integerForKey:@"MBXShowsUserLocation"];
    return showsUserLocation;
}

- (void)setDebugMask:(MGLMapDebugMaskOptions)debugMask {
    self.debugMask = debugMask;
}

- (MGLMapDebugMaskOptions)debugMask {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSInteger debugMask = [defaults integerForKey:@"MBXDebugMask"];
    return debugMask;
}

- (void)setShowsZoomLevelHUD:(BOOL)showsZoomLevelHUD {
    self.showsZoomLevelHUD = showsZoomLevelHUD;
}

-(BOOL)showsZoomLevelHUD {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    BOOL showsZoomLevelHUD = [defaults integerForKey:@"MBXShowsZoomLevelHUD"];
    return showsZoomLevelHUD;
}

-(void)setShowsTimeFrameGraph:(BOOL)showsTimeFrameGraph {
    self.showsTimeFrameGraph = showsTimeFrameGraph;
}

-(BOOL)showsTimeFrameGraph {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    BOOL showsTimeFrameGraph = [defaults integerForKey:@"MBXShowsTimeFrameGraph"];
    return showsTimeFrameGraph;
}

@end
