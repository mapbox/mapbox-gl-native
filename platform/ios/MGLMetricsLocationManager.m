//
//  MBLocationManager.m
//  Hermes
//
//  Dynamic Settings.bundle loading based on:
//  http://stackoverflow.com/questions/510216/can-you-make-the-settings-in-settings-bundle-default-even-if-you-dont-open-the
//
//  Created by Brad Leege on 3/8/15.
//  Copyright (c) 2015 Mapbox. All rights reserved.
//

#import "MGLMetricsLocationManager.h"
#import "CoreLocation/CoreLocation.h"
#import "MGLMapboxEvents.h"

@interface MGLMetricsLocationManager()

@property (atomic) CLLocationManager *locationManager;

@end

@implementation MGLMetricsLocationManager

static MGLMetricsLocationManager *sharedManager = nil;

- (id) init {
    if (self = [super init]) {
        _locationManager = [[CLLocationManager alloc] init];
        _locationManager.distanceFilter = 2;
        _locationManager.desiredAccuracy = kCLLocationAccuracyBest;
        _locationManager.pausesLocationUpdatesAutomatically = YES;
        [_locationManager setDelegate:self];
    }
    return self;
}

+ (id)sharedManager {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedManager = [[self alloc] init];
    });
    return sharedManager;
}

- (BOOL) isAuthorizedStatusDetermined {
    return ([CLLocationManager authorizationStatus] != kCLAuthorizationStatusNotDetermined);
}

- (void) requestAlwaysAuthorization {
    if ([self.locationManager respondsToSelector:@selector(requestAlwaysAuthorization)]) {
        [self.locationManager requestAlwaysAuthorization];
    } else {
        // This is iOS 7 or below so Starting Location Updates will trigger authorization request
        [self startUpdatingLocation];
    }
}

- (void) startUpdatingLocation {
    [self.locationManager startUpdatingLocation];
}

- (void) stopUpdatingLocation {
    [self.locationManager stopUpdatingLocation];
}

#pragma mark CLLocationManagerDelegate
- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray *)locations {
    //  Iterate through locations to pass all data
    for (CLLocation *loc in locations) {
        NSMutableDictionary *evt = [[NSMutableDictionary alloc] init];
        [evt setValue:[[NSNumber alloc] initWithDouble:loc.coordinate.latitude] forKey:@"lat"];
        [evt setValue:[[NSNumber alloc] initWithDouble:loc.coordinate.longitude] forKey:@"lng"];
        [[MGLMapboxEvents sharedManager] pushEvent:@"location" withAttributes:evt];
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
            [[MGLMetricsLocationManager sharedManager] stopUpdatingLocation];
            break;
        case kCLAuthorizationStatusAuthorized:
            newStatus = @"User Has Authorized / Authorized Always";
            [[MGLMetricsLocationManager sharedManager] startUpdatingLocation];
            break;
            //        case kCLAuthorizationStatusAuthorizedAlways:
            //            newStatus = @"Not Determined";
            //            break;
        case kCLAuthorizationStatusAuthorizedWhenInUse:
            newStatus = @"User Has Authorized When In Use Only";
            [[MGLMetricsLocationManager sharedManager] startUpdatingLocation];
            break;
        default:
            newStatus = @"Unknown";
            break;
    }
}

@end
