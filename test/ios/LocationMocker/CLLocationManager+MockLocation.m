//
// Based on gist by Eric Allam
// https://gist.github.com/ericallam/5689235
//

#import "CLLocationManager+MockLocation.h"
#import "LocationMocker.h"

// This category implements methods that will be swizzled to replace key methods
// in CLLocationManager to mock out the user location.
@implementation CLLocationManager (MockLocation)

// Replaces startUpdatingLocation, will send the locationManager:didUpdateLocations: message
// to the delegate after a wait of 0.1 seconds using dispatch_after to simulate how 
// startUpdatingLocation actually works. This will not simulate any kind of location errors.
- (void)custom_startUpdatingLocation
{
    if (self.delegate)
    {
        if ([self.delegate respondsToSelector:@selector(locationManager:didUpdateLocations:)])
        {
            // delay the locationManager:didUpdateLocations: message 0.1 seconds
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 0.1 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
                NSArray *locations = @[self.location];
                [self.delegate locationManager:self didUpdateLocations:locations];
            });
        }
        
        if ([self.delegate respondsToSelector:@selector(locationManager:didUpdateToLocation:fromLocation:)])
        {
            // delay the locationManager:didUpdateToLocation:fromLocation: message 0.1 seconds
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 0.1 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
                #pragma clang diagnostic push
                #pragma clang diagnostic ignored "-Wdeprecated-declarations"
                #pragma clang diagnostic ignored "-Wnonnull"
                [self.delegate locationManager:self didUpdateToLocation:self.location fromLocation:nil];
                #pragma clang diagnostic pop
            });
        }
    }
}

// Replaces location, returns the mocked CLLocation object
- (CLLocation *)custom_location;
{
    return [[CLLocation alloc] initWithLatitude:kMockedLatitude longitude:kMockedLongitude];
}


// Replaces startUpdatingHeading, sends locationManager:didUpdateHeading:
- (void)custom_startUpdatingHeading
{
    if ([self.delegate respondsToSelector:@selector(locationManager:didUpdateHeading:)])
    {
        // delay the locationManager:didUpdateHeading: message 0.1 seconds
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 0.1 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
            [self.delegate locationManager:self didUpdateHeading:self.heading];
        });
    }
}

// Replaces heading, returns the mocked CLHeading object
- (CLHeading *)custom_heading;
{
    CLHeadingStruct heading;
    heading.accuracy = kMockedHeadingAccuracy;
    heading.trueHeading = kMockedHeadingTrueHeading;
    heading.magneticHeading = kMockedHeadingMagneticHeading;
    heading.x = kMockedHeadingX;
    heading.y = kMockedHeadingY;
    heading.z = kMockedHeadingZ;
    
    return [[CLHeading alloc] initWithClientHeading:heading];
}

+ (BOOL)custom_headingAvailable
{
    return YES;
}

@end

