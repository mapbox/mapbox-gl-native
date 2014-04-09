#import "MBXSettings.h"

#import <Foundation/Foundation.h>

using namespace llmr;

Settings_iOS::Settings_iOS() {
    [[NSUserDefaults standardUserDefaults] registerDefaults:@{
                                                                @"longitude" : @(0),
                                                                @"latitude" : @(0),
                                                                @"scale" : @(1),
                                                                @"angle" : @(0),
                                                                @"debug" : @(false)
                                                            }];
}

Configuration Settings_iOS::load() {
    Configuration config;
    NSDictionary *settings = [[NSUserDefaults standardUserDefaults] dictionaryRepresentation];

    config.longitude = [settings[@"longitude"] doubleValue];
    config.latitude = [settings[@"latitude"] doubleValue];
    config.scale = [settings[@"scale"] doubleValue];
    config.angle = [settings[@"angle"] doubleValue];
    config.debug = [settings[@"debug"] boolValue];
    return config;
}

void Settings_iOS::persist(const Configuration &config) {
    [[NSUserDefaults standardUserDefaults]
        setValuesForKeysWithDictionary:@{
                                           @"longitude" : @(config.longitude),
                                           @"latitude" : @(config.latitude),
                                           @"scale" : @(config.scale),
                                           @"angle" : @(config.angle),
                                           @"debug" : @(config.debug)
                                       }];
}

void Settings_iOS::sync() { [[NSUserDefaults standardUserDefaults] synchronize]; }

void Settings_iOS::clear() { [NSUserDefaults resetStandardUserDefaults]; }