#ifndef LLMR_JSON_SETTINGS
#define LLMR_JSON_SETTINGS

#include <llmr/map/settings.hpp>

namespace llmr {

class Settings_JSON : public Settings {
public:
    Settings_JSON();
    virtual Configuration load() = 0;
    virtual void persist(const Configuration &config) = 0;
    virtual void sync(const Configuration &config) = 0;
    virtual void clear() = 0;
};

}

#endif
