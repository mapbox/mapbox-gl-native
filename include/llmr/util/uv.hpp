#ifndef LLMR_UTIL_UV
#define LLMR_UTIL_UV

#include <uv.h>
#include <cassert>

namespace uv {

class mutex {
public:
    inline mutex() { assert(uv_mutex_init(&mtx) == 0); }
    inline ~mutex() { uv_mutex_destroy(&mtx); }
    inline void lock() { uv_mutex_lock(&mtx); }
    inline void unlock() { uv_mutex_unlock(&mtx); }
    inline bool trylock() { return uv_mutex_trylock(&mtx); }

private:
    uv_mutex_t mtx;
};

class lock {
public:
    lock(mutex &mtx) : mtx(mtx) { mtx.lock(); }
    ~lock() { mtx.unlock(); }

private:
    mutex &mtx;
};
}

#endif
