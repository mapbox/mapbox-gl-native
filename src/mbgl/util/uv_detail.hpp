#ifndef MBGL_UTIL_UV_DETAIL
#define MBGL_UTIL_UV_DETAIL

#include <mbgl/util/uv.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <uv.h>

#include <functional>
#include <cassert>
#include <memory>
#include <string>

#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10

// Add thread local storage to libuv API:
// https://github.com/joyent/libuv/commit/5d2434bf71e47802841bad218d521fa254d1ca2d

typedef pthread_key_t uv_key_t;

UV_EXTERN int uv_key_create(uv_key_t* key);
UV_EXTERN void uv_key_delete(uv_key_t* key);
UV_EXTERN void* uv_key_get(uv_key_t* key);
UV_EXTERN void uv_key_set(uv_key_t* key, void* value);

#endif


namespace uv {

class loop : public mbgl::util::noncopyable {
public:
    inline loop() {
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
        l = uv_loop_new();
        if (l == nullptr) {
#else
        l = new uv_loop_t;
        if (uv_loop_init(l) != 0) {
#endif
            throw std::runtime_error("failed to initialize loop");
        }
    }

    inline ~loop() {
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
        uv_loop_delete(l);
#else
        uv_loop_close(l);
        delete l;
#endif
    }

    inline void run() {
        uv_run(l, UV_RUN_DEFAULT);
    }

    inline uv_loop_t* operator*() {
        return l;
    }

    inline uv_loop_t* get() {
        return l;
    }

private:
    uv_loop_t *l = nullptr;
};

template <class T>
class handle : public mbgl::util::noncopyable {
public:
    inline handle() : t(reinterpret_cast<uv_handle_t*>(new T)) {
        t->data = this;
    }

    inline ~handle() {
        uv_close(t.release(), [](uv_handle_t* h) {
            delete reinterpret_cast<T*>(h);
        });
    }

    inline void ref() {
        uv_ref(t.get());
    }

    inline void unref() {
        uv_unref(t.get());
    }

    inline T* get() {
        return reinterpret_cast<T*>(t.get());
    }

private:
    std::unique_ptr<uv_handle_t> t;
};

class async : public handle<uv_async_t> {
public:
    inline async(uv_loop_t* loop, std::function<void ()> fn_)
        : fn(fn_) {
        if (uv_async_init(loop, get(), async_cb) != 0) {
            throw std::runtime_error("failed to initialize async");
        }
    }

    inline void send() {
        if (uv_async_send(get()) != 0) {
            throw std::runtime_error("failed to async send");
        }
    }

private:
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
    static void async_cb(uv_async_t* a, int) {
#else
    static void async_cb(uv_async_t* a) {
#endif
        reinterpret_cast<async*>(a->data)->fn();
    }

    std::function<void ()> fn;
};

class timer : public handle<uv_timer_t> {
public:
    inline timer(uv_loop_t* loop) {
        if (uv_timer_init(loop, get()) != 0) {
            throw std::runtime_error("failed to initialize timer");
        }
    }

    inline void start(uint64_t timeout, uint64_t repeat, std::function<void ()> fn_) {
        fn = fn_;
        if (uv_timer_start(get(), timer_cb, timeout, repeat) != 0) {
            throw std::runtime_error("failed to start timer");
        }
    }

    inline void stop() {
        fn = nullptr;
        if (uv_timer_stop(get()) != 0) {
            throw std::runtime_error("failed to stop timer");
        }
    }

private:
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
    static void timer_cb(uv_timer_t* t, int) {
#else
    static void timer_cb(uv_timer_t* t) {
#endif
        reinterpret_cast<timer*>(t->data)->fn();
    }

    std::function<void ()> fn;
};

class mutex : public mbgl::util::noncopyable {
public:
    inline mutex() {
        if (uv_mutex_init(&mtx) != 0) {
            throw std::runtime_error("failed to initialize mutex lock");
        }
    }
    inline ~mutex() { uv_mutex_destroy(&mtx); }
    inline void lock() { uv_mutex_lock(&mtx); }
    inline void unlock() { uv_mutex_unlock(&mtx); }
private:
    uv_mutex_t mtx;
};

class rwlock : public mbgl::util::noncopyable {
public:
    inline rwlock() {
        if (uv_rwlock_init(&mtx) != 0) {
            throw std::runtime_error("failed to initialize read-write lock");
        }
    }
    inline ~rwlock() { uv_rwlock_destroy(&mtx); }
    inline void rdlock() { uv_rwlock_rdlock(&mtx); }
    inline void wrlock() { uv_rwlock_wrlock(&mtx); }
    inline void rdunlock() { uv_rwlock_rdunlock(&mtx); }
    inline void wrunlock() { uv_rwlock_wrunlock(&mtx); }

private:
    uv_rwlock_t mtx;
};

template <class T>
class tls : public mbgl::util::noncopyable {
public:
    inline tls() {
        if (uv_key_create(&key) != 0) {
            throw std::runtime_error("failed to initialize thread local storage key");
        }
    }
    inline ~tls() { uv_key_delete(&key); }
    inline T* get() { return reinterpret_cast<T*>(uv_key_get(&key)); }
    inline void set(T* val) { uv_key_set(&key, val); }

private:
    uv_key_t key;
};

}

#endif
