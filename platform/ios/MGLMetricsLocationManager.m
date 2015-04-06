#import "MGLMapboxEvents.h"
#import "MGLMetricsLocationManager.h"

#import <CoreLocation/CoreLocation.h>

@interface MGLMetricsLocationManager() <CLLocationManagerDelegate>

@property (nonatomic) CLLocationManager *locationManager;

@end

@implementation MGLMetricsLocationManager

- (instancetype) init {
    if (self = [super init]) {
        _locationManager = [[CLLocationManager alloc] init];
        _locationManager.distanceFilter = 10;
        [_locationManager setDelegate:self];
    }
    return self;
}

+ (instancetype)sharedManager {
    static dispatch_once_t onceToken;
    static MGLMetricsLocationManager *sharedManager;
    dispatch_once(&onceToken, ^{
        sharedManager = [[self alloc] init];
    });
    return sharedManager;
}

+ (void) startUpdatingLocation {
    [[MGLMetricsLocationManager sharedManager].locationManager startUpdatingLocation];
}

+ (void) stopUpdatingLocation {
    [[MGLMetricsLocationManager sharedManager].locationManager stopUpdatingLocation];
}

#pragma mark CLLocationManagerDelegate
- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray *)locations {
    //  Iterate through locations to pass all data
    for (CLLocation *loc in locations) {
        [MGLMapboxEvents pushEvent:MGLEventTypeLocation withAttributes:@{
            MGLEventKeyLatitude: @(loc.coordinate.latitude),
            MGLEventKeyLongitude: @(loc.coordinate.longitude),
            MGLEventKeySpeed: @(loc.speed),
            MGLEventKeyCourse: @(loc.course)
        }];
    }
}

- (void)locationManager:(CLLocationManager *)manager didChangeAuthorizationStatus:(CLAuthorizationStatus)status {
    NSString *newStatus = nil;
    switch (status) {
        case kCLAuthorizationStatusNotDetermined:
            newStatus = @"User Hasn't Determined Yet";
            break;
        case kCLAuthorizationStatusRestricted:
            newStatus = @"Restricted and Can't Be Changed By User";
            break;
        case kCLAuthorizationStatusDenied:
            newStatus = @"User Explcitly Denied";
            [MGLMetricsLocationManager stopUpdatingLocation];
            break;
        case kCLAuthorizationStatusAuthorized:
            newStatus = @"User Has Authorized / Authorized Always";
            [MGLMetricsLocationManager startUpdatingLocation];
            break;
            //        case kCLAuthorizationStatusAuthorizedAlways:
            //            newStatus = @"Not Determined";
            //            break;
        case kCLAuthorizationStatusAuthorizedWhenInUse:
            newStatus = @"User Has Authorized When In Use Only";
            [MGLMetricsLocationManager startUpdatingLocation];
            break;
        default:
            newStatus = @"Unknown";
            break;
    }
}

@end
