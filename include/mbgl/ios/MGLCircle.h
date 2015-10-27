#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLShape.h"
#import "MGLOverlay.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLCircle : MGLShape <MGLOverlay>

@property (nonatomic, assign) CLLocationCoordinate2D coordinate;
@property (nonatomic, readonly) CLLocationDistance radius;
@property (nonatomic, readonly) MGLCoordinateBounds overlayBounds;

+ (instancetype)circleWithCenterCoordinate:(CLLocationCoordinate2D)coordinate
                                    radius:(CLLocationDistance)radius;

+ (instancetype)circleWithOverlayBounds:(MGLCoordinateBounds)overlayBounds;

@end

NS_ASSUME_NONNULL_END
