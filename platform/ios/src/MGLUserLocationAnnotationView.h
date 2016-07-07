#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>
#import <Mapbox/Mapbox.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLMapView;
@class MGLUserLocation;

/** View representing an `MGLUserLocation` on screen. */
@interface MGLUserLocationAnnotationView : MGLAnnotationView

@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic, readonly, nullable) MGLUserLocation *userLocation;

- (instancetype)initInMapView:(MGLMapView *)mapView userLocation:(MGLUserLocation *)userLocation;

- (void)didUpdateUserLocation:(MGLUserLocation *)userLocation;

@end

NS_ASSUME_NONNULL_END
