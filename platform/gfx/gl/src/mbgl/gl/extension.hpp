#pragma once

#include <initializer_list>
#include <utility>
#include <functional>

namespace mbgl {
namespace gl {

using ProcAddress = void (*)();

template <class>
class ExtensionFunction;

template <class R, class... Args>
class ExtensionFunction<R(Args...)> {
public:
    ExtensionFunction(const ProcAddress ptr_) : ptr(ptr_) {
    }

    explicit operator bool() const {
        return ptr;
    }

    R operator()(Args... args) const {
        return (*reinterpret_cast<R (*)(Args...)>(ptr))(std::forward<Args>(args)...);
    }

private:
    const ProcAddress ptr;
};

} // namespace gl
} // namespace mbgl
