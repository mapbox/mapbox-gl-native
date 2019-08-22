#import "MBXState.h"

NSString *const MBXCamera = @"MBXCamera";
NSString *const MBXUserTrackingMode = @"MBXUserTrackingMode";
NSString *const MBXShowsUserLocation = @"MBXShowsUserLocation";
NSString *const MBXDebugMaskValue = @"MBXDebugMaskValue";
NSString *const MBXShowsZoomLevelOrnament =  @"MBXShowsZoomLevelOrnament";
NSString *const MBXShowsTimeFrameGraph = @"MBXShowsFrameTimeGraph";
NSString *const MBXShowsMapScale = @"MBXMapShowsScale";
NSString *const MBXMapShowsHeadingIndicator = @"MBXMapShowsHeadingIndicator";
NSString *const MBXMapFramerateMeasurementEnabled = @"MBXMapFramerateMeasurementEnabled";
NSString *const MBXReuseQueueStatsEnabled = @"MBXReuseQueueStatsEnabled";

@interface MBXState()

@end

@implementation MBXState

- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeObject:_camera forKey:MBXCamera];
    [coder encodeObject:[NSNumber numberWithUnsignedInteger:_userTrackingMode] forKey:MBXUserTrackingMode];
    [coder encodeBool:_showsUserLocation forKey:MBXShowsUserLocation];
    [coder encodeObject:[NSNumber numberWithUnsignedInteger:_debugMask] forKey:MBXDebugMaskValue];
    [coder encodeBool:_showsZoomLevelOrnament forKey:MBXShowsZoomLevelOrnament];
    [coder encodeBool:_showsTimeFrameGraph forKey:MBXShowsTimeFrameGraph];
    [coder encodeBool:_showsMapScale forKey:MBXShowsMapScale];
    [coder encodeBool:_showsUserHeadingIndicator forKey:MBXMapShowsHeadingIndicator];
    [coder encodeBool:_framerateMeasurementEnabled forKey:MBXMapFramerateMeasurementEnabled];
    [coder encodeBool:_reuseQueueStatsEnabled forKey:MBXReuseQueueStatsEnabled];
}

- (nullable instancetype)initWithCoder:(nonnull NSCoder *)decoder {
    if (self = [super init]) {
        MGLMapCamera *decodedCamera = [decoder decodeObjectForKey:MBXCamera];
        NSNumber *decodedUserTrackingMode = [decoder decodeObjectForKey:MBXUserTrackingMode];
        BOOL decodedShowsUserLocation = [decoder decodeBoolForKey:MBXShowsUserLocation];
        NSNumber *decodedDebugMaskOptions = [decoder decodeObjectForKey:MBXDebugMaskValue];
        BOOL decodedZoomLevelOrnament = [decoder decodeBoolForKey:MBXShowsZoomLevelOrnament];
        BOOL decodedShowsTimeFrameGraph = [decoder decodeBoolForKey:MBXShowsTimeFrameGraph];
        BOOL decodedShowsMapScale = [decoder decodeBoolForKey:MBXShowsMapScale];
        BOOL decodedShowsUserHeadingIndicator = [decoder decodeBoolForKey:MBXMapShowsHeadingIndicator];
        BOOL decodedFramerateMeasurementEnabled = [decoder decodeBoolForKey:MBXMapFramerateMeasurementEnabled];
        BOOL decodedReuseQueueStatsEnabled = [decoder decodeBoolForKey:MBXReuseQueueStatsEnabled];

        _camera = decodedCamera;
        _userTrackingMode = decodedUserTrackingMode.intValue;
        _showsUserLocation = decodedShowsUserLocation;
        _debugMask = decodedDebugMaskOptions.intValue;
        _showsZoomLevelOrnament = decodedZoomLevelOrnament;
        _showsTimeFrameGraph = decodedShowsTimeFrameGraph;
        _showsMapScale = decodedShowsMapScale;
        _showsUserHeadingIndicator = decodedShowsUserHeadingIndicator;
        _framerateMeasurementEnabled = decodedFramerateMeasurementEnabled;
        _reuseQueueStatsEnabled = decodedReuseQueueStatsEnabled;
    }

    return self;
}

+ (BOOL)supportsSecureCoding {
    return YES;
}

- (NSString *)debugDescription {
    return [NSString stringWithFormat:@"Camera: %@\nTracking mode: %lu\nShows user location: %@\nDebug mask value: %lu\nShows zoom level ornament: %@\nShows time frame graph: %@\nShows map scale: %@\nShows user heading indicator: %@\nFramerate measurement enabled: %@\nReuse queue stats enabled: %@",
            self.camera,
            (unsigned long)self.userTrackingMode,
            self.showsUserLocation ? @"YES" : @"NO",
            (unsigned long)self.debugMask,
            self.showsZoomLevelOrnament ? @"YES" : @"NO",
            self.showsTimeFrameGraph ? @"YES" : @"NO",
            self.showsMapScale ? @"YES" : @"NO",
            self.showsUserHeadingIndicator ? @"YES" : @"NO",
            self.framerateMeasurementEnabled ? @"YES" : @"NO",
            self.reuseQueueStatsEnabled ? @"YES" : @"NO"];
}

@end
