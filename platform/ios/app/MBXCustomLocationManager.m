//
//  MBXCustomLocationManager.m
//  iosapp
//
//  Created by Fabian Guerra Soto on 5/11/18.
//  Copyright © 2018 Mapbox. All rights reserved.
//

#import "MBXCustomLocationManager.h"

@interface MBXCustomLocationManager()
@property (nonatomic) CLDeviceOrientation orientation;
@property (nonatomic, weak) id<MGLLocationManagerDelegate>delegate;
@property (nonatomic, strong) NSTimer *locationTimer;
@property (nonatomic, strong) NSDictionary *coordiantes;
@property (nonatomic) NSUInteger index;
@end

@implementation MBXCustomLocationManager

- (instancetype)init
{
    if (self = [super init]) {
        _coordiantes = [self JSONCoordinates];
        _index = 0;
    }
    return self;
}

- (CLAuthorizationStatus)authorizationStatus {
    return kCLAuthorizationStatusAuthorizedAlways;
}

- (CLDeviceOrientation)headingOrientation {
    return _orientation;
}

- (void)requestAlwaysAuthorization {
    
}

- (void)requestWhenInUseAuthorization {
    
}

- (void)setDelegate:(id<MGLLocationManagerDelegate>)delegate {
    _delegate = delegate;
}

- (void)setHeadingOrientation:(CLDeviceOrientation)headingOrientation {
    _orientation = headingOrientation;
}

- (void)startUpdatingHeading {
    
}

- (void)startUpdatingLocation {
    self.locationTimer = [NSTimer scheduledTimerWithTimeInterval:0.5 target:self selector:@selector(updateLocation) userInfo:nil repeats:YES];
}

- (void)stopUpdatingHeading {
    
    
}

- (void)stopUpdatingLocation {
    [self.locationTimer invalidate];
    self.locationTimer = nil;
}

- (void)updateLocation
{
    if ([self.delegate respondsToSelector:@selector(locationManager:didUpdateLocations:)]) {
        
        NSArray *coors = [self.coordiantes objectForKey:@"coordinates"];
        if (self.index >= [coors count] ) {
            self.index = 0;
        }
        NSDictionary *loc = coors[self.index];
        CLLocationDegrees latitude = [[loc objectForKey:@"latitude"] doubleValue];
        CLLocationDegrees longitude = [[loc objectForKey:@"longitude"] doubleValue];
        CLLocation *location = [[CLLocation alloc] initWithLatitude:latitude longitude:longitude];
        self.index++;
        [self.delegate locationManager:self didUpdateLocations:@[location]];
    }
}

- (NSDictionary *)JSONCoordinates
{
    NSString *path = [[NSBundle mainBundle] pathForResource:@"coordinates" ofType:@"json"];
    NSData *data = [NSData dataWithContentsOfFile:path];
    return [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:nil];
}

@end
