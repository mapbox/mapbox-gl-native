#ifndef LLMR_COMMON_SETTINGS_NSUSERDEFAULTS
#define LLMR_COMMON_SETTINGS_NSUSERDEFAULTS

namespace llmr {

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
