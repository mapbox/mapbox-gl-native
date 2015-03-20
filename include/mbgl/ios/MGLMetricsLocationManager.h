//
//  MBLocationManager.h
//  Hermes
//
//  Created by Brad Leege on 3/8/15.
//  Copyright (c) 2015 Mapbox. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CoreLocation/CoreLocation.h"

@interface MGLMetricsLocationManager : NSObject <CLLocationManagerDelegate>

+ (id)sharedManager;

- (BOOL) isAuthorizedStatusDetermined;
- (void) requestAlwaysAuthorization;

- (void) startUpdatingLocation;
- (void) stopUpdatingLocation;

@end
