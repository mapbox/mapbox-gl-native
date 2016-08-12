#import "MGLUserLocationAnnotationView.h"
#import "MGLUserLocation.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLMapView;

@interface MGLUserLocationAnnotationView (Private)

@property (nonatomic, weak, nullable) MGLUserLocation *userLocation;
@property (nonatomic, weak, nullable) MGLMapView *mapView;

@end

NS_ASSUME_NONNULL_END
