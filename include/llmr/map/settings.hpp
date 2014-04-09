#ifndef LLMR_MAP_SETTINGS
#define LLMR_MAP_SETTINGS

#include <llmr/util/noncopyable.hpp>
#include <llmr/map/configuration.hpp>

namespace llmr {

class Settings : private util::noncopyable {
public:
    virtual Configuration load() = 0;    // load into memory
    virtual void persist(const Configuration &config) = 0; // persist in memory (then call sync() if platform-appropriate)
    virtual void sync() = 0;    // save to disk
    virtual void clear() = 0;   // clear from memory and disk
    virtual ~Settings() {}
public:
};

}

#endif
