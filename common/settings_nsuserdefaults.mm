#import <Foundation/Foundation.h>

#include "settings_nsuserdefaults.hpp"

using namespace llmr;

Settings_NSUserDefaults::Settings_NSUserDefaults()
{
    [[NSUserDefaults standardUserDefaults] registerDefaults:@{ @"longitude" : @(longitude),
                                                               @"latitude"  : @(latitude),
                                                               @"zoom"      : @(zoom),
                                                               @"angle"     : @(angle),
                                                               @"debug"     : @(debug) }];
    load();
}

void Settings_NSUserDefaults::load()
{
    NSDictionary *settings = [[NSUserDefaults standardUserDefaults] dictionaryRepresentation];

    longitude = [settings[@"longitude"] doubleValue];
    latitude  = [settings[@"latitude"]  doubleValue];
    zoom      = [settings[@"zoom"]      doubleValue];
    angle     = [settings[@"angle"]     doubleValue];
    debug     = [settings[@"debug"]     boolValue];
}

void Settings_NSUserDefaults::save()
{
    [[NSUserDefaults standardUserDefaults] setValuesForKeysWithDictionary:@{ @"longitude" : @(longitude),
                                                                             @"latitude"  : @(latitude),
                                                                             @"zoom"     : @(zoom),
                                                                             @"angle"     : @(angle),
                                                                             @"debug"     : @(debug) }];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

void Settings_NSUserDefaults::clear()
{
    [NSUserDefaults resetStandardUserDefaults];
}