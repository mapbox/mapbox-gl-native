//
//  MGLLocationManager.m
//  dynamic
//
//  Created by Fabian Guerra Soto on 5/7/18.
//  Copyright © 2018 Mapbox. All rights reserved.
//

#import "MGLLocationManager.h"

@interface MGLAppleLocationManager()<CLLocationManagerDelegate>

@property (nonatomic) CLLocationManager *locationManager;
@property (nonatomic, weak) id<MGLLocationManagerDelegate> delegate;

@end

@implementation MGLAppleLocationManager

- (instancetype)init
{
    if (self = [super init]) {
        _locationManager = [[CLLocationManager alloc] init];
        _locationManager.delegate = self;
    }
    return self;
}

- (void)startUpdatingLocation
{
    [self.locationManager startUpdatingLocation];
}

- (void)startUpdatingHeading
{
    [self.locationManager startUpdatingHeading];
}

- (void)stopUpdatingLocation
{
    [self.locationManager stopUpdatingLocation];
}

- (void)stopUpdatingHeading
{
    [self.locationManager stopUpdatingHeading];
}

- (void)requestAlwaysAuthorization
{
    [self.locationManager requestAlwaysAuthorization];
}

- (void)requestWhenInUseAuthorization
{
    [self.locationManager requestWhenInUseAuthorization];
}

- (void)setDelegate:(id<MGLLocationManagerDelegate>)delegate
{
    _delegate = delegate;
}

- (void)setHeadingOrientation:(CLDeviceOrientation)headingOrientation
{
    self.locationManager.headingOrientation = headingOrientation;
}

- (CLDeviceOrientation)headingOrientation
{
    return self.locationManager.headingOrientation;
}

- (CLAuthorizationStatus)authorizationStatus
{
    return [CLLocationManager authorizationStatus];
}

#pragma mark - User Location

- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray<CLLocation *> *)locations
{
    if ([self.delegate respondsToSelector:@selector(locationManager:didUpdateLocations:)]) {
        [self.delegate locationManager:self didUpdateLocations:locations];
    }
}

@end
