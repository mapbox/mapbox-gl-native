#import "MGLLocationManager_Private.h"

@implementation CLLocationManager (MGLAdditions)

- (CLAuthorizationStatus)authorizationStatus
{
    return [CLLocationManager authorizationStatus];
}

@end
