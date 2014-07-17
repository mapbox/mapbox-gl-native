#ifndef MBGL_COMMON_SETTINGS_NSUSERDEFAULTS
#define MBGL_COMMON_SETTINGS_NSUSERDEFAULTS

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
    double angle = 0;

    bool debug = false;
};

}

#endif
