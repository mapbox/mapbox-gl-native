#ifndef LLMR_MAP_SETTINGS
#define LLMR_MAP_SETTINGS

#include <llmr/util/noncopyable.hpp>

namespace llmr {

class Settings : private util::noncopyable {
public:
    virtual void save() = 0;
    virtual ~Settings() {}
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
