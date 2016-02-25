#import "MGLLocationManager.h"
#import <UIKit/UIKit.h>

static const NSTimeInterval fiveMinuteTimeInterval = 300.0;
static const NSTimeInterval fiveSecondTimeInterval = 5.0;
static const CLLocationDistance regionRadiusLocationDistance = 300.0;
static NSString * const MGLLocationManagerRegionIdentifier = @"MGLLocationManagerRegionIdentifier.fence.center";

@interface MGLLocationManager ()

@property (nonatomic) CLLocationManager *standardLocationManager;
@property (nonatomic) BOOL hostAppHasBackgroundCapability;
@property (nonatomic, getter=isUpdatingLocation) BOOL updatingLocation;
@property (nonatomic) NSDate *backgroundLocationServiceTimeoutAllowedDate;
@property (nonatomic) NSTimer *backgroundLocationServiceTimeoutTimer;

@end

@implementation MGLLocationManager

- (instancetype)init {
    self = [super init];
    if (self) {
        NSArray *backgroundModes = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"UIBackgroundModes"];
       _hostAppHasBackgroundCapability = [backgroundModes containsObject:@"location"];
    }
    return self;
}

- (void)startUpdatingLocation {
    if ([self isUpdatingLocation]) {
        return;
    }
    
    [self configurePassiveStandardLocationManager];
    [self startLocationServices];
}

- (void)stopUpdatingLocation {
    if ([self isUpdatingLocation]) {
        [self.standardLocationManager stopUpdatingLocation];
        [self.standardLocationManager stopMonitoringSignificantLocationChanges];
        self.updatingLocation = NO;
        if ([self.delegate respondsToSelector:@selector(locationManagerDidStopLocationUpdates:)]) {
            [self.delegate locationManagerDidStopLocationUpdates:self];
        }
    }
}

#pragma mark - Utilities

- (void)configurePassiveStandardLocationManager {
    if (!self.standardLocationManager) {
        CLLocationManager *standardLocationManager = [[CLLocationManager alloc] init];
        standardLocationManager.delegate = self;
        standardLocationManager.desiredAccuracy = kCLLocationAccuracyThreeKilometers;
        standardLocationManager.distanceFilter = 1;
        self.standardLocationManager = standardLocationManager;
    }
}

- (void)startLocationServices {
    if ([CLLocationManager authorizationStatus] == kCLAuthorizationStatusAuthorized ||
        [CLLocationManager authorizationStatus] == kCLAuthorizationStatusAuthorizedWhenInUse) {
       
        // If the host app can run in the background with `always` location permissions then allow background
        // updates and start the significant location change service and background timeout timer
        if (self.hostAppHasBackgroundCapability && [CLLocationManager authorizationStatus] == kCLAuthorizationStatusAuthorized) {
            [self.standardLocationManager startMonitoringSignificantLocationChanges];
            [self startBackgroundTimeoutTimer];
            // On iOS 9 and above also allow background location updates
            if ([self.standardLocationManager respondsToSelector:@selector(allowsBackgroundLocationUpdates)]) {
                self.standardLocationManager.allowsBackgroundLocationUpdates = YES;
            }
        }
        
        [self.standardLocationManager startUpdatingLocation];
        self.updatingLocation = YES;
        if ([self.delegate respondsToSelector:@selector(locationManagerDidStartLocationUpdates:)]) {
            [self.delegate locationManagerDidStartLocationUpdates:self];
        }
    }
}

- (void)timeoutAllowedCheck {
    if (self.backgroundLocationServiceTimeoutAllowedDate == nil) {
        return;
    }
  
    if ([UIApplication sharedApplication].applicationState == UIApplicationStateActive ||
        [UIApplication sharedApplication].applicationState == UIApplicationStateInactive ) {
        [self startBackgroundTimeoutTimer];
        return;
    }
    
    NSTimeInterval timeIntervalSinceTimeoutAllowed = [[NSDate date] timeIntervalSinceDate:self.backgroundLocationServiceTimeoutAllowedDate];
    if (timeIntervalSinceTimeoutAllowed > 0) {
        [self.standardLocationManager stopUpdatingLocation];
        self.backgroundLocationServiceTimeoutAllowedDate = nil;
        if ([self.delegate respondsToSelector:@selector(locationManagerBackgroundLocationUpdatesDidTimeout:)]) {
            [self.delegate locationManagerBackgroundLocationUpdatesDidTimeout:self];
        }
    }
}

- (void)startBackgroundTimeoutTimer {
    [self.backgroundLocationServiceTimeoutTimer invalidate];
    self.backgroundLocationServiceTimeoutAllowedDate = [[NSDate date] dateByAddingTimeInterval:fiveMinuteTimeInterval];
    self.backgroundLocationServiceTimeoutTimer = [NSTimer scheduledTimerWithTimeInterval:fiveSecondTimeInterval target:self selector:@selector(timeoutAllowedCheck) userInfo:nil repeats:YES];
}

- (void)establishRegionMonitoringForLocation:(CLLocation *)location {
    CLCircularRegion *region = [[CLCircularRegion alloc] initWithCenter:location.coordinate radius:regionRadiusLocationDistance identifier:MGLLocationManagerRegionIdentifier];
    region.notifyOnEntry = NO;
    region.notifyOnExit = YES;
    [self.standardLocationManager startMonitoringForRegion:region];
}

#pragma mark - CLLocationManagerDelegate

- (void)locationManager:(CLLocationManager *)manager didChangeAuthorizationStatus:(CLAuthorizationStatus)status {
    switch (status) {
        case kCLAuthorizationStatusAuthorized: // Also handles kCLAuthorizationStatusAuthorizedAlways
        case kCLAuthorizationStatusAuthorizedWhenInUse:
            [self startUpdatingLocation];
            break;
        default:
            [self stopUpdatingLocation];
            break;
    }
}

- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray<CLLocation *> *)locations {
    CLLocation *location = locations.lastObject;
    if (location.speed > 0.0) {
        [self startBackgroundTimeoutTimer];
    }
    if (self.standardLocationManager.monitoredRegions.count == 0 || location.horizontalAccuracy < regionRadiusLocationDistance) {
        [self establishRegionMonitoringForLocation:location];
    }
    if ([self.delegate respondsToSelector:@selector(locationManager:didUpdateLocations:)]) {
        [self.delegate locationManager:self didUpdateLocations:locations];
    }
}

- (void)locationManager:(CLLocationManager *)manager didExitRegion:(CLRegion *)region {
    [self startBackgroundTimeoutTimer];
    [self.standardLocationManager startUpdatingLocation];
}

- (void)locationManagerDidPauseLocationUpdates:(CLLocationManager *)manager {
    if ([UIApplication sharedApplication].applicationState == UIApplicationStateBackground) {
        if ([self.delegate respondsToSelector:@selector(locationManagerBackgroundLocationUpdatesDidAutomaticallyPause:)]) {
            [self.delegate locationManagerBackgroundLocationUpdatesDidAutomaticallyPause:self];
        }
    }
}

@end
