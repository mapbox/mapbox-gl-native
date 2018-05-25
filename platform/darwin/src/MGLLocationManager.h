#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol MGLLocationManagerDelegate;

/**
 The `MGLLocationManager` protocol defines a set of methods that you
 use to receive location-related events.
 */
@protocol MGLLocationManager <NSObject>

@required

@property (nonatomic, weak) id<MGLLocationManagerDelegate> delegate;
@property (nonatomic) CLDeviceOrientation headingOrientation;
@property (nonatomic, readonly) CLAuthorizationStatus authorizationStatus;

- (void)startUpdatingLocation;
- (void)startUpdatingHeading;

- (void)stopUpdatingLocation;
- (void)stopUpdatingHeading;

- (void)requestAlwaysAuthorization;
- (void)requestWhenInUseAuthorization;

@end

/**
 The `MGLLocationManagerDelegate` protocol defines a set of methods that you
 use to receive location updates from the associated location manager.
 */
@protocol MGLLocationManagerDelegate <NSObject>

- (void)locationManager:(id<MGLLocationManager>)manager
     didUpdateLocations:(NSArray<CLLocation *> *)locations;

- (void)locationManager:(id<MGLLocationManager>)manager
       didUpdateHeading:(CLHeading *)newHeading;

@optional

@end

NS_ASSUME_NONNULL_END
