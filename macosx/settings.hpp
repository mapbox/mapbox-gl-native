#ifndef LLMR_MACOSX_SETTINGS
#define LLMR_MACOSX_SETTINGS

#include <llmr/map/settings.hpp>

namespace llmr {

class macosx_settings : public settings {
public:
    macosx_settings();
    virtual void save();
    virtual void load();
    virtual void clear();
};

}

#endif
