#pragma once

#import <mbgl/ios/MGLTypes.h>

namespace mbgl {

class Settings_NSUserDefaults {
public:
    Settings_NSUserDefaults();
    void load();
    void save();
    void clear();

public:
    double longitude = 0;
    double latitude = 0;
    double zoom = 0;
    double bearing = 0;
    double pitch = 0;

    MGLUserTrackingMode userTrackingMode = MGLUserTrackingModeNone;
    bool showsUserLocation = false;

    uint32_t debug = 0;
};

}
