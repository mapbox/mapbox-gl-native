#ifndef MBGL_STORAGE_DEFAULT_THREAD_CONTEXT
#define MBGL_STORAGE_DEFAULT_THREAD_CONTEXT

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/uv.hpp>

#include <uv.h>
#include <pthread.h>

#include <map>
#include <cassert>

namespace mbgl {

// This is a template class that provides a per-thread and per-loop Context object. It can be used
// by implementations to store global state.

template <typename Context>
class ThreadContext : private util::noncopyable {
protected:
    MBGL_STORE_THREAD(tid)
    using Map = std::map<uv_loop_t *, std::unique_ptr<Context>>;

public:
    static Context *Get(uv_loop_t *loop);

private:
    static pthread_key_t key;
    static pthread_once_t once;

public:
    ThreadContext(uv_loop_t *loop);
    ~ThreadContext();

public:
    uv_loop_t *loop;
};

template <typename Context>
Context *ThreadContext<Context>::Get(uv_loop_t *loop) {
    pthread_once(&once, []() {
        pthread_key_create(&key, [](void *ptr) {
            assert(ptr);
            delete reinterpret_cast<Map *>(ptr);
        });
    });
    auto contexts = reinterpret_cast<Map *>(pthread_getspecific(key));
    if (!contexts) {
        contexts = new Map();
        pthread_setspecific(key, contexts);
    }

    // Now find a ThreadContext that matches the requested loop.
    auto it = contexts->find(loop);
    if (it == contexts->end()) {
        auto result = contexts->emplace(loop, util::make_unique<Context>(loop));
        assert(result.second); // Make sure it was actually inserted.
        return result.first->second.get();
    } else {
        return it->second.get();
    }
}

template <typename Context>
ThreadContext<Context>::ThreadContext(uv_loop_t *loop_) : loop(loop_) {
}

template <typename Context>
ThreadContext<Context>::~ThreadContext() {
    MBGL_VERIFY_THREAD(tid);
}


}

#endif
