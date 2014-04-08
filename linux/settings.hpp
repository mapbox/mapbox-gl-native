#ifndef LLMR_JSON_SETTINGS
#define LLMR_JSON_SETTINGS

#include <llmr/map/settings.hpp>

namespace llmr {

class Settings_JSON : public Settings {
public:
    Settings_JSON();
    virtual Configuration load();
    virtual void persist(const Configuration &config);
    virtual void sync();
    virtual void clear();

private:
    Configuration configuration;
};

}

#endif
