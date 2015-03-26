#ifndef MBGL_COMMON_SETTINGS_NSUSERDEFAULTS
#define MBGL_COMMON_SETTINGS_NSUSERDEFAULTS

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

    MGLUserTrackingMode userTrackingMode = MGLUserTrackingModeNone;
    bool showsUserLocation = false;

    bool debug = false;
};

}

#endif
