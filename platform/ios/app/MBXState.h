#import <Foundation/Foundation.h>
#import <Mapbox/Mapbox.h>

NS_ASSUME_NONNULL_BEGIN

FOUNDATION_EXTERN NSString *const MBXCamera;
FOUNDATION_EXTERN NSString *const MBXUserTrackingMode;
FOUNDATION_EXTERN NSString *const MBXShowsUserLocation;
FOUNDATION_EXTERN NSString *const MBXDebugMaskValue;
FOUNDATION_EXTERN NSString *const MBXShowsZoomLevelOrnament;
FOUNDATION_EXTERN NSString *const MBXShowsTimeFrameGraph;

@interface MBXState : NSObject

@property (nonatomic, nullable) NSMutableDictionary *state;

@property (nonatomic, nullable) MGLMapCamera *camera;
@property (nonatomic) MGLUserTrackingMode userTrackingMode;
@property (nonatomic) BOOL showsUserLocation;
@property (nonatomic) MGLMapDebugMaskOptions debugMask;
@property (nonatomic) BOOL showsZoomLevelOrnament;
@property (nonatomic) BOOL showsTimeFrameGraph;

@property (nonatomic, strong) NSObject *test;

@end

NS_ASSUME_NONNULL_END
