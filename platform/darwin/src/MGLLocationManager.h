//
//  MGLLocationManager.h
//  dynamic
//
//  Created by Fabian Guerra Soto on 5/7/18.
//  Copyright © 2018 Mapbox. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

@protocol MGLLocationManagerDelegate;

@protocol MGLLocationManager <NSObject>

- (void)startUpdatingLocation;
- (void)startUpdatingHeading;

- (void)stopUpdatingLocation;
- (void)stopUpdatingHeading;

- (void)requestAlwaysAuthorization;
- (void)requestWhenInUseAuthorization;

- (void)setDelegate:(id<MGLLocationManagerDelegate>)delegate;

- (void)setHeadingOrientation:(CLDeviceOrientation)headingOrientation;
- (CLDeviceOrientation)headingOrientation;

- (CLAuthorizationStatus)authorizationStatus;

@optional
@end

@protocol MGLLocationManagerDelegate <NSObject>

- (void)locationManager:(id<MGLLocationManager>)manager
     didUpdateLocations:(NSArray<CLLocation *> *)locations;

- (void)locationManager:(id<MGLLocationManager>)manager
       didUpdateHeading:(CLHeading *)newHeading;

@optional
@end





@interface MGLAppleLocationManager : NSObject<MGLLocationManager>

@end
