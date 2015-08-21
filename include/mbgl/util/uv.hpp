#ifndef MBGL_UTIL_UV
#define MBGL_UTIL_UV

#include <string>
#include <memory>

typedef struct uv_handle_s uv_handle_t;
typedef struct uv_async_s uv_async_t;
typedef struct uv_timer_s uv_timer_t;
typedef struct uv_loop_s uv_loop_t;
typedef struct uv_fs_s uv_fs_t;

namespace uv {

class rwlock;
class loop;
class async;
class mutex;
class cond;

class lock {
public:
    lock(mutex &);
    lock(const std::unique_ptr<mutex> &);
    lock(const lock &) = delete;
    lock(lock &&lock);
    lock &operator=(const lock &lock) = delete;
    lock &operator=(lock &&lock);
    ~lock();
private:
    mutex *mtx = nullptr;
};

class readlock {
public:
    readlock(rwlock &);
    readlock(const std::unique_ptr<rwlock> &);
    readlock(const readlock &) = delete;
    readlock(readlock &&lock);
    readlock &operator=(const readlock &lock) = delete;
    readlock &operator=(readlock &&lock);
    ~readlock();
private:
    rwlock *mtx = nullptr;
};

class writelock {
public:
    writelock(rwlock &);
    writelock(const std::unique_ptr<rwlock> &);
    writelock(const writelock &) = delete;
    writelock(writelock &&lock);
    writelock &operator=(const writelock &lock) = delete;
    writelock &operator=(writelock &&lock);
    ~writelock();
private:
    rwlock *mtx = nullptr;
};

template <class T>
class exclusive {
public:
    exclusive(T& val, mutex &mtx) : ptr(&val), lock(mtx) {}
    exclusive(T *val, mutex &mtx) : ptr(val), lock(mtx) {}
    exclusive(mutex &mtx) : lock(mtx) {}
    exclusive(const std::unique_ptr<mutex> &mtx) : lock(mtx) {}
    exclusive(const exclusive &) = delete;
    exclusive(exclusive &&) = default;
    exclusive &operator=(const exclusive &) = delete;
    exclusive &operator=(exclusive &&) = default;

    T *operator->() { return ptr; }
    const T *operator->() const { return ptr; }
    T *operator*() { return ptr; }
    const T *operator*() const { return ptr; }
    operator T&() { return *ptr; }
    operator const T&() const { return *ptr; }

    void operator<<(T& val) { operator<<(&val); }
    void operator<<(T *val) {
        if (ptr) {
            throw std::runtime_error("exclusive<> was assigned before");
        }
        ptr = val;
    }

private:
    T *ptr = nullptr;
    class lock lock;
};



const char *getFileRequestError(uv_fs_t *req);

template <typename T>
void close(T *specific) {
    uv_close(reinterpret_cast<uv_handle_t *>(specific), [](uv_handle_t *generic) {
        delete reinterpret_cast<T *>(generic);
    });
}

}

#endif
