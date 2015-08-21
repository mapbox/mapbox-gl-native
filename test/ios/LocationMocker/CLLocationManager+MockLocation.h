//
// Based on gist by Eric Allam
// https://gist.github.com/ericallam/5689235
//

#import <CoreLocation/CoreLocation.h>

@interface CLLocationManager (MockLocation)

- (void)custom_startUpdatingLocation;
- (CLLocation *)custom_location;

- (void)custom_startUpdatingHeading;
- (CLHeading *)custom_heading;
+ (BOOL)custom_headingAvailable;

@end

// private setter struct for CLHeading
typedef struct {
    double x;
    double y;
    double z;
    double magneticHeading;
    double trueHeading;
    double accuracy;
    double timestamp;
    double temperature;
    double magnitude;
    double inclination;
    int calibration;
} CLHeadingStruct;

// create reference to private API method
@interface CLHeading ()

- (id)initWithClientHeading:(CLHeadingStruct)heading;

@end
