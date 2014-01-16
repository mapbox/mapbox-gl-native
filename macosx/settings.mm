#import <Foundation/Foundation.h>

#include "settings.hpp"


using namespace llmr;

macosx_settings::macosx_settings() {
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

void macosx_settings::load() {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

    // position
    longitude = [defaults doubleForKey:@"longitude"];
    latitude = [defaults doubleForKey:@"latitude"];
    scale = [defaults doubleForKey:@"scale"];
    angle = [defaults doubleForKey:@"angle"];

    // debug
    debug = [defaults boolForKey:@"debug"];
}

void macosx_settings::save() {
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

void macosx_settings::clear() {
    // TODO
}
