#ifndef LLMR_UTIL_UV
#define LLMR_UTIL_UV

#include <uv.h>
#include <functional>
#include <cassert>
#include <boost/lockfree/queue.hpp>

namespace uv {

typedef void (*callback)();

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

class once {
public:
    void operator()(void (*callback)(void)) {
        uv_once(&o, callback);
    }

private:
    uv_once_t o = UV_ONCE_INIT;
};

}

#endif
