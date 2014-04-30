#ifndef LLMR_JSON_SETTINGS
#define LLMR_JSON_SETTINGS

namespace llmr {

class Settings_JSON {
public:
    Settings_JSON();
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
