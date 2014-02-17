#ifndef LLMR_MACOSX_SETTINGS
#define LLMR_MACOSX_SETTINGS

#include <llmr/map/settings.hpp>

namespace llmr {

class Settings_MacOSX : public Settings {
public:
    Settings_MacOSX();
    virtual void load();
    virtual void persist();
    virtual void sync();
    virtual void clear();
    double angle;
    double longitude;
    double latitude;
    double scale;
};

}

#endif
