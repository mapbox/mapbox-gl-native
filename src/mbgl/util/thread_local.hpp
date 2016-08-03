#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <memory>

namespace mbgl {
namespace util {

template <class T>
class ThreadLocal : public noncopyable {
public:
    ThreadLocal(T* val) {
        ThreadLocal();
        set(val);
    }

    ThreadLocal();
    ~ThreadLocal();

    T* get();
    void set(T* ptr);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace util
} // namespace mbgl
