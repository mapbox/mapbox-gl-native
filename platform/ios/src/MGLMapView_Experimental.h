#import <Mapbox/Mapbox.h>

@interface MGLMapView (Experimental)

#pragma mark Rendering Performance Measurement

/** Enable rendering performance measurement. */
@property (nonatomic) BOOL experimental_enableFrameRateMeasurement;

/**
 Average frames per second over the previous second, updated once per second.

 Requires `experimental_enableFrameRateMeasurement`.
 */
@property (nonatomic, readonly) CGFloat averageFrameRate;

/**
  Frame render duration for the previous frame, updated instantaneously.

  Requires `experimental_enableFrameRateMeasurement`.
 */
@property (nonatomic, readonly) CFTimeInterval frameTime;

/**
 Average frame render duration over the previous second, updated once per
 second.

 Requires `experimental_enableFrameRateMeasurement`.
 */
@property (nonatomic, readonly) CFTimeInterval averageFrameTime;

@end
