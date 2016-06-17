#pragma once

#include <memory>
#include <mutex>


namespace mbgl {
namespace util {

template <class T>
class exclusive {
public:
    exclusive(T* val, std::unique_ptr<std::lock_guard<std::mutex>> mtx) : ptr(val), lock(std::move(mtx)) {}

    T* operator->() { return ptr; }
    const T* operator->() const { return ptr; }
    T* operator*() { return ptr; }
    const T* operator*() const { return ptr; }

private:
    T *ptr;
    std::unique_ptr<std::lock_guard<std::mutex>> lock;
};


} // end namespace util
} // end namespace mbgl
