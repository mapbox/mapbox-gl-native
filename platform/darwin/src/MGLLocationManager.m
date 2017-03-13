#import "MGLLocationManager_Private.h"

@interface MGLCLLocationManager()<CLLocationManagerDelegate>

@property (nonatomic) CLLocationManager *locationManager;

@end

@implementation MGLCLLocationManager

- (instancetype)init
{
    if (self = [super init]) {
        _locationManager = [[CLLocationManager alloc] init];
        _locationManager.delegate = self;
    }
    return self;
}

@synthesize delegate;

#if !TARGET_OS_TV
- (void)setHeadingOrientation:(CLDeviceOrientation)headingOrientation
{
    self.locationManager.headingOrientation = headingOrientation;
}

- (CLDeviceOrientation)headingOrientation
{
    return self.locationManager.headingOrientation;
}
#endif

- (void)setDesiredAccuracy:(CLLocationAccuracy)desiredAccuracy {
    self.locationManager.desiredAccuracy = desiredAccuracy;
}

- (CLLocationAccuracy)desiredAccuracy {
    return self.locationManager.desiredAccuracy;
}

- (CLAuthorizationStatus)authorizationStatus {
    return [CLLocationManager authorizationStatus];
}

#if !TARGET_OS_TV
- (void)setActivityType:(CLActivityType)activityType {
    self.locationManager.activityType = activityType;
}

- (CLActivityType)activityType {
    return self.locationManager.activityType;
}

- (void)dismissHeadingCalibrationDisplay {
    [self.locationManager dismissHeadingCalibrationDisplay];
}

- (void)requestAlwaysAuthorization {
    [self.locationManager requestAlwaysAuthorization];
}
#endif

- (void)requestWhenInUseAuthorization {
    [self.locationManager requestWhenInUseAuthorization];
}

#if !TARGET_OS_TV
- (void)startUpdatingHeading {
    [self.locationManager startUpdatingHeading];
}

- (void)startUpdatingLocation {
    [self.locationManager startUpdatingLocation];
}

- (void)stopUpdatingHeading {
    [self.locationManager stopUpdatingHeading];
}
#endif

- (void)stopUpdatingLocation {
    [self.locationManager stopUpdatingLocation];
}

- (void)dealloc
{
    [self.locationManager stopUpdatingLocation];
#if !TARGET_OS_TV
    [self.locationManager stopUpdatingHeading];
#endif
    self.locationManager.delegate = nil;
    self.delegate = nil;
}

#pragma mark - CLLocationManagerDelegate

- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray<CLLocation *> *)locations {
    if ([self.delegate respondsToSelector:@selector(locationManager:didUpdateLocations:)]) {
        [self.delegate locationManager:self didUpdateLocations:locations];
    }
}

#if !TARGET_OS_TV
- (void)locationManager:(CLLocationManager *)manager didUpdateHeading:(CLHeading *)newHeading {
    if ([self.delegate respondsToSelector:@selector(locationManager:didUpdateHeading:)]) {
        [self.delegate locationManager:self didUpdateHeading:newHeading];
    }
}

- (BOOL)locationManagerShouldDisplayHeadingCalibration:(CLLocationManager *)manager {
    if ([self.delegate respondsToSelector:@selector(locationManagerShouldDisplayHeadingCalibration:)]) {
        return [self.delegate locationManagerShouldDisplayHeadingCalibration:self];
    }
    
    return NO;
}
#endif

- (void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error {
    if ([self.delegate respondsToSelector:@selector(locationManager:didFailWithError:)]) {
        [self.delegate locationManager:self didFailWithError:error];
    }
}

@end
