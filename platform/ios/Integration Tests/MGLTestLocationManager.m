#import "MGLTestLocationManager.h"

// Used to supply integration tests with a simulated location manager.
// Methods that are empty are not used within integration tests and are
// therefore unimplemented.

@implementation MGLTestLocationManager

@synthesize delegate;

- (CLAuthorizationStatus)authorizationStatus { return kCLAuthorizationStatusAuthorizedAlways; }

- (void)setHeadingOrientation:(CLDeviceOrientation)headingOrientation { }

- (CLDeviceOrientation)headingOrientation { return 90; }

- (void)requestAlwaysAuthorization { }

- (void)requestWhenInUseAuthorization { }

- (void)startUpdatingHeading { }

// Simulate one location update
- (void)startUpdatingLocation
{
    if ([self.delegate respondsToSelector:@selector(locationManager:didUpdateLocations:)]) {
        CLLocation *location = [[CLLocation alloc] initWithLatitude:37.787357 longitude:-122.39899];
        [self.delegate locationManager:self didUpdateLocations:@[location]];
    }
}

- (void)stopUpdatingHeading { }

- (void)stopUpdatingLocation { }

- (void)dismissHeadingCalibrationDisplay { }

- (void)dealloc { self.delegate = nil; }

- (void)locationManager:(CLLocationManager *)manager didUpdateHeading:(CLHeading *)newHeading { }

- (BOOL)locationManagerShouldDisplayHeadingCalibration:(CLLocationManager *)manager { return NO; }

@end
