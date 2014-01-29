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

Settings_iOS::Settings_iOS() {
    NSDictionary *appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:

                                 // position
                                 [NSNumber numberWithDouble:longitude], @"longitude",
                                 [NSNumber numberWithDouble:latitude], @"latitude",
                                 [NSNumber numberWithDouble:scale], @"scale",
                                 [NSNumber numberWithDouble:angle], @"angle",

                                 // debug
                                 [NSNumber numberWithBool:debug], @"debug",

                                 nil
                                 ];

    [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
}

void Settings_iOS::load() {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

    // position
    longitude = [defaults doubleForKey:@"longitude"];
    latitude = [defaults doubleForKey:@"latitude"];
    scale = [defaults doubleForKey:@"scale"];
    angle = [defaults doubleForKey:@"angle"];

    // debug
    debug = [defaults boolForKey:@"debug"];
}

void Settings_iOS::save() {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

    NSDictionary *appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:

                                 // position
                                 [NSNumber numberWithDouble:longitude], @"longitude",
                                 [NSNumber numberWithDouble:latitude], @"latitude",
                                 [NSNumber numberWithDouble:scale], @"scale",
                                 [NSNumber numberWithDouble:angle], @"angle",
                                 
                                 // debug
                                 [NSNumber numberWithBool:debug], @"debug",
                                 
                                 nil
                                 ];
//    [defaults setPersistentDomain:appDefaults forName:[[NSBundle mainBundle] bundleIdentifier]];

//    [defaults synchronize];
}

void Settings_iOS::clear() {
}