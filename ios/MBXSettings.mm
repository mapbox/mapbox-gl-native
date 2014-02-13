//
//  MBXSettings.cpp
//  llmr
//
//  Created by Justin R. Miller on 1/27/14.
//
//

#import "MBXSettings.h"

#import <Foundation/Foundation.h>

using namespace llmr;

Settings_iOS::Settings_iOS()
{
    [[NSUserDefaults standardUserDefaults] registerDefaults:@{ @"longitude" : @(longitude),
                                                               @"latitude"  : @(latitude),
                                                               @"scale"     : @(scale),
                                                               @"angle"     : @(angle),
                                                               @"debug"     : @(debug) }];
}

void Settings_iOS::load()
{
    NSDictionary *settings = [[NSUserDefaults standardUserDefaults] dictionaryRepresentation];

    longitude = [settings[@"longitude"] doubleValue];
    latitude  = [settings[@"latitude"]  doubleValue];
    scale     = [settings[@"scale"]     doubleValue];
    angle     = [settings[@"angle"]     doubleValue];
    debug     = [settings[@"debug"]     boolValue];
}

void Settings_iOS::persist()
{
    [[NSUserDefaults standardUserDefaults] setValuesForKeysWithDictionary:@{ @"longitude" : @(longitude),
                                                                             @"latitude"  : @(latitude),
                                                                             @"scale"     : @(scale),
                                                                             @"angle"     : @(angle),
                                                                             @"debug"     : @(debug) }];
}

void Settings_iOS::sync()
{
    [[NSUserDefaults standardUserDefaults] synchronize];
}

void Settings_iOS::clear()
{
    [NSUserDefaults resetStandardUserDefaults];
}