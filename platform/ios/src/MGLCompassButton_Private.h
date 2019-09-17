#import <UIKit/UIKit.h>

#import <Mapbox/MGLCompassButton.h>

@class MGLMapView;

NS_ASSUME_NONNULL_BEGIN

@interface MGLCompassButton (Private)

+ (instancetype)compassButtonWithMapView:(MGLMapView *)mapView;

@property (nonatomic, weak) MGLMapView *mapView;

- (void)updateCompass;

@end

NS_ASSUME_NONNULL_END
