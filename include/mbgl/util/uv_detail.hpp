#ifndef MBGL_UTIL_UV_DETAIL
#define MBGL_UTIL_UV_DETAIL

#include <mbgl/util/uv-worker.h>
#include <mbgl/util/noncopyable.hpp>

#include <uv.h>

#include <functional>
#include <cassert>
#include <memory>
#include <string>

namespace uv {

template <class T>
void close(T* handle) {
    uv_close(reinterpret_cast<uv_handle_t*>(handle), nullptr);
}

class thread : public mbgl::util::noncopyable {
public:
    inline thread(std::function<void ()> fn_)
        : fn(fn_) {
        if (uv_thread_create(&t, thread_cb, this) != 0) {
            throw std::runtime_error("failed to initialize thread");
        }
    }

    void join() {
        if (uv_thread_join(&t) != 0) {
            throw std::runtime_error("failed to join thred");
        }
    }

private:
    static void thread_cb(void* data) {
        reinterpret_cast<thread*>(data)->fn();
    }

    uv_thread_t t;
    std::function<void ()> fn;
};

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

    inline uv_loop_t *operator*() { return l; }

private:
    uv_loop_t *l = nullptr;
};

class async : public mbgl::util::noncopyable {
public:
    inline async(uv_loop_t* loop, std::function<void ()> fn_)
        : fn(fn_) {
        a.data = this;
        if (uv_async_init(loop, &a, async_cb) != 0) {
            throw std::runtime_error("failed to initialize async");
        }
    }

    inline ~async() {
        close(&a);
    }

    inline void send() {
        if (uv_async_send(&a) != 0) {
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

    uv_async_t a;
    std::function<void ()> fn;
};

class mutex : public mbgl::util::noncopyable {
    friend class cond;

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

class cond : public mbgl::util::noncopyable {
public:
    inline cond() {
        if (uv_cond_init(&cnd) != 0) {
            throw std::runtime_error("failed to initialize condition variable");
        }
    }
    inline ~cond() { uv_cond_destroy(&cnd); }
    inline void signal() { uv_cond_signal(&cnd); }
    inline void broadcast() { uv_cond_broadcast(&cnd); }
    inline void wait(mutex &mtx) { uv_cond_wait(&cnd, &mtx.mtx); }
    inline void timedwait(mutex &mtx, uint64_t timeout) { uv_cond_timedwait(&cnd, &mtx.mtx, timeout); }

private:
    uv_cond_t cnd;
};

class lock : public mbgl::util::noncopyable {
public:
    lock(mutex &mtx_) : mtx(mtx_) { mtx.lock(); }
    ~lock() { mtx.unlock(); }

private:
    mutex &mtx;
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

class readlock : public mbgl::util::noncopyable {
public:
    inline readlock(rwlock &mtx_) : mtx(mtx_) { mtx.rdlock(); }
    inline readlock(const std::unique_ptr<rwlock> &mtx_) : mtx(*mtx_) { mtx.rdlock(); }
    inline ~readlock() { mtx.rdunlock(); }

private:
    rwlock &mtx;
};

class writelock : public mbgl::util::noncopyable {
public:
    inline writelock(rwlock &mtx_) : mtx(mtx_) { mtx.wrlock(); }
    inline writelock(const std::unique_ptr<rwlock> &mtx_) : mtx(*mtx_) { mtx.wrlock(); }
    inline ~writelock() { mtx.wrunlock(); }

private:
    rwlock &mtx;
};

class worker : public mbgl::util::noncopyable {
public:
    inline worker(uv_loop_t *loop, unsigned int count, const char *name = nullptr) : w(new uv_worker_t) {
        uv_worker_init(w, loop, count, name);
    }
    inline ~worker() {
        uv_worker_close(w, [](uv_worker_t *worker_) {
            delete worker_;
        });
    }
    inline void add(void *data, uv_worker_cb work_cb, uv_worker_after_cb after_work_cb) {
        uv_worker_send(w, data, work_cb, after_work_cb);
    }

private:
    uv_worker_t *w;
};

template <typename T>
class work : public mbgl::util::noncopyable {
public:
    typedef std::function<void (T&)> work_callback;
    typedef std::function<void (T&)> after_work_callback;

    template<typename... Args>
    work(worker &worker, work_callback work_cb_, after_work_callback after_work_cb_, Args&&... args)
        : data(std::forward<Args>(args)...),
          work_cb(work_cb_),
          after_work_cb(after_work_cb_) {
        worker.add(this, do_work, after_work);
    }

private:
    static void do_work(void *data) {
        work<T> *w = reinterpret_cast<work<T> *>(data);
        w->work_cb(w->data);
    }

    static void after_work(void *data) {
        work<T> *w = reinterpret_cast<work<T> *>(data);
        w->after_work_cb(w->data);
        delete w;
    }

private:
    T data;
    work_callback work_cb;
    after_work_callback after_work_cb;
};

}

#endif
