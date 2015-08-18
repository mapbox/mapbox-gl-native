#import <Foundation/Foundation.h>

#include <mbgl/platform/darwin/settings_nsuserdefaults.hpp>

using namespace mbgl;

Settings_NSUserDefaults::Settings_NSUserDefaults()
{
    [[NSUserDefaults standardUserDefaults] registerDefaults:@{
        @"longitude"         : @(longitude),
        @"latitude"          : @(latitude),
        @"zoom"              : @(zoom),
        @"bearing"           : @(bearing),
        @"pitch"             : @(pitch),
        @"userTrackingMode"  : @(userTrackingMode),
        @"showsUserLocation" : @(showsUserLocation),
        @"debug"             : @(debug),
    }];
    load();
}

void Settings_NSUserDefaults::load()
{
    NSDictionary *settings = [[NSUserDefaults standardUserDefaults] dictionaryRepresentation];

    longitude = [settings[@"longitude"] doubleValue];
    latitude  = [settings[@"latitude"]  doubleValue];
    zoom      = [settings[@"zoom"]      doubleValue];
    bearing   = [settings[@"bearing"]   doubleValue];
    pitch     = [settings[@"pitch"]     doubleValue];
    debug     = [settings[@"debug"]     boolValue];
    
    unsigned uncheckedTrackingMode = [settings[@"trackingMode"] unsignedIntValue];
    if (uncheckedTrackingMode > MGLUserTrackingModeNone &&
        uncheckedTrackingMode <= MGLUserTrackingModeFollowWithCourse)
    {
        userTrackingMode = (MGLUserTrackingMode)uncheckedTrackingMode;
    }
    showsUserLocation = [settings[@"showsUserLocation"] boolValue];
}

void Settings_NSUserDefaults::save()
{
    [[NSUserDefaults standardUserDefaults] setValuesForKeysWithDictionary:@{
        @"longitude"         : @(longitude),
        @"latitude"          : @(latitude),
        @"zoom"              : @(zoom),
        @"bearing"           : @(bearing),
        @"pitch"             : @(pitch),
        @"userTrackingMode"  : @(userTrackingMode),
        @"showsUserLocation" : @(showsUserLocation),
        @"debug"             : @(debug),
    }];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

void Settings_NSUserDefaults::clear()
{
    [NSUserDefaults resetStandardUserDefaults];
}