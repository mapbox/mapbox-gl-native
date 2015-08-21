#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLMapView;
@class MGLUserLocation;

/** View representing an MGLUserLocation on screen. */
@interface MGLUserLocationAnnotationView : UIView

@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic) MGLUserLocation *annotation;
@property (nonatomic, readonly, nullable) CALayer *haloLayer;

- (instancetype)initInMapView:(MGLMapView *)mapView NS_DESIGNATED_INITIALIZER;
- (void)setupLayers;

@end

NS_ASSUME_NONNULL_END
