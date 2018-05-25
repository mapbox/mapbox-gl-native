#import "MGLLocationManager_Private.h"

@interface MGLCLLocationManager()<CLLocationManagerDelegate>

@property (nonatomic) CLLocationManager *locationManager;

@end

@implementation MGLCLLocationManager

@synthesize delegate;

- (instancetype)init
{
    if (self = [super init]) {
        _locationManager = [[CLLocationManager alloc] init];
        _locationManager.delegate = self;
    }
    return self;
}

- (CLAuthorizationStatus)authorizationStatus
{
    return [CLLocationManager authorizationStatus];
}

- (void)setHeadingOrientation:(CLDeviceOrientation)headingOrientation
{
    _locationManager.headingOrientation = headingOrientation;
}

- (CLDeviceOrientation)headingOrientation
{
    return _locationManager.headingOrientation;
}

- (void)requestAlwaysAuthorization
{
    [self.locationManager requestAlwaysAuthorization];
}

- (void)requestWhenInUseAuthorization
{
    [self.locationManager requestWhenInUseAuthorization];
}

- (void)startUpdatingHeading
{
    [self.locationManager startUpdatingHeading];
}

- (void)startUpdatingLocation
{
    [self.locationManager startUpdatingLocation];
}

- (void)stopUpdatingHeading
{
    [self.locationManager stopUpdatingHeading];
}

- (void)stopUpdatingLocation
{
    [self.locationManager stopUpdatingLocation];
}

#pragma mark - CLLocationManagerDelegate

- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray<CLLocation *> *)locations
{
    if ([self.delegate respondsToSelector:@selector(locationManager:didUpdateLocations:)]) {
        [self.delegate locationManager:self didUpdateLocations:locations];
    }
}

- (void)locationManager:(CLLocationManager *)manager didUpdateHeading:(CLHeading *)newHeading
{
    if ([self.delegate respondsToSelector:@selector(locationManager:didUpdateHeading:)]) {
        [self.delegate locationManager:self didUpdateHeading:newHeading];
    }
}

@end
