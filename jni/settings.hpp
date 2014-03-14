#ifndef LLMR_ANDROID_SETTINGS
#define LLMR_ANDROID_SETTINGS

#include <llmr/map/settings.hpp>

namespace llmr {

class Settings_Android : public Settings {
public:
    Settings_Android();
    virtual void load();
    virtual void persist();
    virtual void sync();
    virtual void clear();
};

}

#endif

