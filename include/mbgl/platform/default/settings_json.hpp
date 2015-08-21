#ifndef MBGL_JSON_SETTINGS
#define MBGL_JSON_SETTINGS

namespace mbgl {

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
    double bearing = 0;

    bool debug = false;
};

}

#endif
