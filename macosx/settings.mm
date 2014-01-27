#import <Foundation/Foundation.h>

#include "settings.hpp"


using namespace llmr;

Settings_MacOSX::Settings_MacOSX() {
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

void Settings_MacOSX::load() {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

    // position
    longitude = [defaults doubleForKey:@"longitude"];
    latitude = [defaults doubleForKey:@"latitude"];
    scale = [defaults doubleForKey:@"scale"];
    angle = [defaults doubleForKey:@"angle"];

    // debug
    debug = [defaults boolForKey:@"debug"];
}

void Settings_MacOSX::save() {
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
    [defaults setPersistentDomain:appDefaults forName:[[NSBundle mainBundle] bundleIdentifier]];

    [defaults synchronize];
}

void Settings_MacOSX::clear() {
}
