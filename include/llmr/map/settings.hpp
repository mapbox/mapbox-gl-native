#ifndef LLMR_MAP_SETTINGS
#define LLMR_MAP_SETTINGS

#include <llmr/util/noncopyable.hpp>

namespace llmr {

class Settings : private util::noncopyable {
public:
    virtual void load() {};    // load into memory
    virtual void persist() {}; // persist in memory (then call sync() if platform-appropriate)
    virtual void sync() {};    // save to disk
    virtual void clear() {};   // clear from memory and disk
    virtual ~Settings() {}

public:
    double longitude = 0;
    double latitude = 0;
    double scale = 0;
    double angle = 0;

    bool debug = false;
};

}

#endif
