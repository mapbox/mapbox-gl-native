#ifndef MBGL_UTIL_UV_DETAIL
#define MBGL_UTIL_UV_DETAIL

#include <uv.h>
#include <functional>
#include <cassert>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

#include <boost/lockfree/queue.hpp>

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <string>


namespace uv {

class thread {
public:
    inline operator uv_thread_t *() { return &t; }

private:
    uv_thread_t t;
};

class loop {
public:
    inline loop() {
        if (uv_loop_init(&l) != 0) {
            throw std::runtime_error("failed to initialize loop");
        }
    }

    inline ~loop() { uv_loop_close(&l); }

    inline uv_loop_t *operator*() { return &l; }

private:
    uv_loop_t l;
};

class mutex {
public:
    inline mutex() {
        if (uv_mutex_init(&mtx) != 0) {
            throw std::runtime_error("failed to initialize mutex");
        }
    }
    inline ~mutex() { uv_mutex_destroy(&mtx); }
    inline void lock() { uv_mutex_lock(&mtx); }
    inline void unlock() { uv_mutex_unlock(&mtx); }

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

class rwlock {
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

class readlock {
public:
    inline readlock(rwlock &mtx) : mtx(mtx) { mtx.rdlock(); }
    inline readlock(const std::unique_ptr<rwlock> &mtx) : mtx(*mtx) { mtx->rdlock(); }
    inline ~readlock() { mtx.rdunlock(); }

private:
    rwlock &mtx;
};

class writelock {
public:
    inline writelock(rwlock &mtx) : mtx(mtx) { mtx.wrlock(); }
    inline writelock(const std::unique_ptr<rwlock> &mtx) : mtx(*mtx) { mtx->wrlock(); }
    inline ~writelock() { mtx.wrunlock(); }

private:
    rwlock &mtx;
};

class once {
public:
    typedef void (*callback)();
    void operator()(void (*callback)(void)) {
        uv_once(&o, callback);
    }

private:
    uv_once_t o = UV_ONCE_INIT;
};

template <typename T>
class work {
public:
    typedef void (*work_callback)(T &object);
    typedef void (*after_work_callback)(T &object);

    template<typename... Args>
    work(const std::shared_ptr<loop> &loop, work_callback work_cb, after_work_callback after_work_cb, Args&&... args)
        : loop(loop),
          data(std::forward<Args>(args)...),
          work_cb(work_cb),
          after_work_cb(after_work_cb) {
        req.data = this;
        uv_queue_work(**loop, &req, do_work, after_work);
    }

private:
    static void do_work(uv_work_t *req) {
        work<T> *w = static_cast<work<T> *>(req->data);
        w->work_cb(w->data);
    }

    static void after_work(uv_work_t *req, int) {
        work<T> *w = static_cast<work<T> *>(req->data);
        w->after_work_cb(w->data);
        delete w;
    }

private:
    std::shared_ptr<uv::loop> loop;
    uv_work_t req;
    T data;
    work_callback work_cb;
    after_work_callback after_work_cb;
};

}

#endif
