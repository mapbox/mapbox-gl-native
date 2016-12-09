#import <CoreLocation/CoreLocation.h>

#import "MGLTypes.h"

@protocol MGLLocationManagerDelegate;

MGL_EXTERN
@interface MGLLocationManager : NSObject <CLLocationManagerDelegate>

@property (nonatomic, weak) id<MGLLocationManagerDelegate> delegate;

- (void)startUpdatingLocation;
- (void)stopUpdatingLocation;

@end

@protocol MGLLocationManagerDelegate <NSObject>

@optional

- (void)locationManager:(MGLLocationManager *)locationManager didUpdateLocations:(NSArray *)locations;
- (void)locationManagerDidStartLocationUpdates:(MGLLocationManager *)locationManager;
- (void)locationManagerBackgroundLocationUpdatesDidTimeout:(MGLLocationManager *)locationManager;
- (void)locationManagerBackgroundLocationUpdatesDidAutomaticallyPause:(MGLLocationManager *)locationManager;
- (void)locationManagerDidStopLocationUpdates:(MGLLocationManager *)locationManager;

@end
