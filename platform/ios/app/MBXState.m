#import "MBXState.h"

NSString *const MBXCamera = @"MBXCamera";
NSString *const MBXUserTrackingMode = @"MBXUserTrackingMode";
NSString *const MBXShowsUserLocation = @"MBXShowsUserLocation";
NSString *const MBXDebugMask = @"MBXDebugMaskValue";
NSString *const MBXShowsZoomLevelHUD =  @"MBXShowsZoomLevelOrnament";
NSString *const MBXShowsTimeFrameGraph = @"MBXShowsFrameTimeGraph";

@interface MBXState()

@property NSUserDefaults *standardUserDefaults;
@property MGLMapView *mapView;

@end

@implementation MBXState

- (instancetype)initForMapView:(MGLMapView*)mapView {
    if (self = [super init]) {

        NSMutableDictionary *dictionary = [NSMutableDictionary dictionaryWithObjectsAndKeys: @"", MBXCamera,
            @"", MBXUserTrackingMode,
            @"", MBXShowsUserLocation,
            @"", MBXDebugMask,
            @"", MBXShowsZoomLevelHUD,
            @"", MBXShowsTimeFrameGraph,
            nil];

        _state = dictionary;
        _standardUserDefaults = [NSUserDefaults standardUserDefaults];
        _mapView = mapView;
    }
    return self;
}


// Current map camera
- (void)setCamera:(MGLMapCamera *)camera {
    NSData *archivedCamera = [NSKeyedArchiver archivedDataWithRootObject:self.mapView.camera];
    [self.standardUserDefaults setObject:archivedCamera forKey:MBXCamera];
    _camera = camera;
}

- (MGLMapCamera*)getCamera {
    NSData *archivedCamera = [self.standardUserDefaults objectForKey:MBXCamera];
    MGLMapCamera *unarchivedCamera = archivedCamera ? [NSKeyedUnarchiver unarchiveObjectWithData:archivedCamera] : nil;
    return unarchivedCamera;
}

// User tracking mode status
- (void)setUserTrackingMode:(MGLUserTrackingMode)userTrackingMode {
    [self.standardUserDefaults setInteger:self.mapView.userTrackingMode forKey:MBXUserTrackingMode];
    _userTrackingMode = userTrackingMode;
}

- (MGLUserTrackingMode)getUserTrackingMode {
    if (_userTrackingMode >= 0 &&
        _userTrackingMode >= MGLUserTrackingModeNone &&
        _userTrackingMode <= MGLUserTrackingModeFollowWithCourse)
    {
        return _userTrackingMode;
    }

    return MGLUserTrackingModeNone;
}

// User location status
- (void)setShowsUserLocation:(BOOL)showsUserLocation {
    [self.standardUserDefaults setBool:self.mapView.showsUserLocation forKey:MBXShowsUserLocation];
    _showsUserLocation = showsUserLocation;
}

- (BOOL)getShowsUserLocation {
    return [self.standardUserDefaults boolForKey:MBXShowsUserLocation];
}

// Debug mask value
- (void)setDebugMask:(MGLMapDebugMaskOptions)debugMask {
    [self.standardUserDefaults setInteger:self.mapView.debugMask forKey:MBXDebugMaskValue];
    _debugMask = debugMask;
}

- (MGLMapDebugMaskOptions)getDebugMask {
    return [self.standardUserDefaults integerForKey:MBXDebugMaskValue];
}

// Zoom level ornament
- (void)setShowsZoomLevelOrnament:(BOOL)showsZoomLevelOrnament {
    [self.standardUserDefaults setBool:self.mapView.showsZoomLevelOrnament forKey:MBXShowsZoomLevelOrnament];
    _showsZoomLevelOrnament = showsZoomLevelOrnament;
}

-(BOOL)getShowsZoomLevelHUD {
    return [self.standardUserDefaults boolForKey:MBXShowsZoomLevelOrnament];
}

// Time frame graph
-(void)setShowsTimeFrameGraph:(BOOL)showsTimeFrameGraph {
    [self.standardUserDefaults setBool:self.mapView.showsTimeFrameGraph forKey:MBXShowsTimeFrameGraph];
    _showsTimeFrameGraph = showsTimeFrameGraph;
}

-(BOOL)getShowsTimeFrameGraph {
    return [self.standardUserDefaults boolForKey:MBXShowsTimeFrameGraph];
}

@end
