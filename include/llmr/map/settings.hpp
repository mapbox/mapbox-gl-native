#ifndef LLMR_MAP_SETTINGS
#define LLMR_MAP_SETTINGS

namespace llmr {

class settings {
public:
    virtual void save() = 0;

public:
    // position
    double longitude = 0;
    double latitude = 0;
    double scale = 0;
    double angle = 0;

    // debug
    bool debug = false;
};

}

#endif
