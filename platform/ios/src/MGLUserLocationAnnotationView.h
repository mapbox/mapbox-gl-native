#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLTypes.h"
#import "MGLAnnotationView.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLMapView;
@class MGLUserLocation;

/** View representing an `MGLUserLocation` on screen. */
@interface MGLUserLocationAnnotationView : MGLAnnotationView

@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic, readonly, nullable) MGLUserLocation *userLocation;
@property (nonatomic, readonly, nullable) CALayer *haloLayer;
@property (nonatomic, readonly) CALayer *hitTestLayer;

- (instancetype)initInMapView:(MGLMapView *)mapView NS_DESIGNATED_INITIALIZER;
- (void)setupLayers;

@end

NS_ASSUME_NONNULL_END
