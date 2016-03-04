#import <Foundation/Foundation.h>

#import "MGLDownloadRegion.h"
#import "MGLGeometry.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLTilePyramidDownloadRegion : NSObject <MGLDownloadRegion>

@property (nonatomic, readonly) MGLCoordinateBounds bounds;
@property (nonatomic, readonly) double minimumZoomLevel;
@property (nonatomic, readonly) double maximumZoomLevel;

- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithStyleURL:(nullable NSURL *)styleURL bounds:(MGLCoordinateBounds)bounds fromZoomLevel:(double)minimumZoomLevel toZoomLevel:(double)maximumZoomLevel NS_DESIGNATED_INITIALIZER;

@end

NS_ASSUME_NONNULL_END
