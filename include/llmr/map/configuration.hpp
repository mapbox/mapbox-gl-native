#ifndef LLMR_MAP_CONFIGURATION
#define LLMR_MAP_CONFIGURATION

namespace llmr {

class Configuration {
public:
    double longitude = 0;
    double latitude = 0;
    double scale = 1;
    double angle = 0;
    bool debug;
};

}

#endif
