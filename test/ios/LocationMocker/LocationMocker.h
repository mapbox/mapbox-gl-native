//
// Based on gist by Eric Allam
// https://gist.github.com/ericallam/5689235
//

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

// lat and long of the mocked current location (Mapbox San Francisco)
static const CLLocationDegrees kMockedLatitude = 37.775716;
static const CLLocationDegrees kMockedLongitude = -122.413688;

// heading (values pulled from south-facing device)
static const double kMockedHeadingAccuracy = 20.0;
static const double kMockedHeadingTrueHeading = 170.53;
static const double kMockedHeadingMagneticHeading = 154.83;
static const double kMockedHeadingX = -7.079;
static const double kMockedHeadingY = -16.548;
static const double kMockedHeadingZ = -44.194;

@interface LocationMocker : NSObject

+ (void)load;

@end
