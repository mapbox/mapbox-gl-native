#ifndef LLMR_MACOSX_SETTINGS
#define LLMR_MACOSX_SETTINGS

#include <llmr/map/settings.hpp>

namespace llmr {

class Settings_MacOSX : public Settings {
public:
    Settings_MacOSX();
    virtual void save();
    virtual void load();
    virtual void clear();
};

}

#endif
