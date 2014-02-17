#include "settings.hpp"

using namespace llmr;

Settings_MacOSX::Settings_MacOSX()
{
    /*
    [[NSUserDefaults standardUserDefaults] registerDefaults:@{ @"longitude" : @(longitude),
                                                               @"latitude"  : @(latitude),
                                                               @"scale"     : @(scale),
                                                               @"angle"     : @(angle),
                                                               @"debug"     : @(debug) }];
                                                               */
}

void Settings_MacOSX::load()
{
    /*
    NSDictionary *settings = [[NSUserDefaults standardUserDefaults] dictionaryRepresentation];

    longitude = [settings[@"longitude"] doubleValue];
    latitude  = [settings[@"latitude"]  doubleValue];
    scale     = [settings[@"scale"]     doubleValue];
    angle     = [settings[@"angle"]     doubleValue];
    debug     = [settings[@"debug"]     boolValue];
    */
}

void Settings_MacOSX::persist()
{
    /*
    [[NSUserDefaults standardUserDefaults] setValuesForKeysWithDictionary:@{ @"longitude" : @(longitude),
                                                                             @"latitude"  : @(latitude),
                                                                             @"scale"     : @(scale),
                                                                             @"angle"     : @(angle),
                                                                             @"debug"     : @(debug) }];
                                                                             */
}

void Settings_MacOSX::sync()
{
    /*
    [[NSUserDefaults standardUserDefaults] synchronize];
    */
}

void Settings_MacOSX::clear()
{
    /*
    [NSUserDefaults resetStandardUserDefaults];
    */
}
