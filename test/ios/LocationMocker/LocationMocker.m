//
// Based on gist by Eric Allam
// https://gist.github.com/ericallam/5689235
//

#import "LocationMocker.h"
#import "CSSwizzler.h"
#import "CLLocationManager+MockLocation.h"

@implementation LocationMocker

+ (void)load
{
    [CSSwizzler swizzleClass:[CLLocationManager class]
               replaceMethod:@selector(startUpdatingLocation)
                  withMethod:@selector(custom_startUpdatingLocation)];
    
    [CSSwizzler swizzleClass:[CLLocationManager class]
               replaceMethod:@selector(location)
                  withMethod:@selector(custom_location)];
    
    [CSSwizzler swizzleClass:[CLLocationManager class]
               replaceMethod:@selector(startUpdatingHeading)
                  withMethod:@selector(custom_startUpdatingHeading)];
    
    [CSSwizzler swizzleClass:[CLLocationManager class]
               replaceMethod:@selector(heading)
                  withMethod:@selector(custom_heading)];
    
    [CSSwizzler swizzleClass:[CLLocationManager class]
          replaceClassMethod:@selector(headingAvailable)
                  withMethod:@selector(custom_headingAvailable)];
}

@end
