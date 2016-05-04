#ifndef MBGL_UTIL_EXCLUSIVE
#define MBGL_UTIL_EXCLUSIVE

#include <memory>
#include <mutex>


namespace mbgl {
namespace util {

template <class T>
class exclusive {
public:
    inline exclusive(T* val, std::unique_ptr<std::lock_guard<std::mutex>> mtx) : ptr(val), lock(std::move(mtx)) {}

    inline T* operator->() { return ptr; }
    inline const T* operator->() const { return ptr; }
    inline T* operator*() { return ptr; }
    inline const T* operator*() const { return ptr; }

private:
    T *ptr;
    std::unique_ptr<std::lock_guard<std::mutex>> lock;
};


} // end namespace util
} // end namespace mbgl

#endif
