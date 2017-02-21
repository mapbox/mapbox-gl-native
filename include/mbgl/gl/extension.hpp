#pragma once

#include <initializer_list>
#include <utility>

namespace mbgl {
namespace gl {

using glProc = void (*)();
void InitializeExtensions(glProc (*getProcAddress)(const char*));

namespace detail {

class ExtensionFunctionBase {
public:
    using Probe = std::pair<const char*, const char*>;
    ExtensionFunctionBase(std::initializer_list<Probe>);
    glProc ptr;
};

} // namespace detail

template <class>
class ExtensionFunction;

template <class R, class... Args>
class ExtensionFunction<R(Args...)> : protected detail::ExtensionFunctionBase {
public:
    using detail::ExtensionFunctionBase::ExtensionFunctionBase;

    explicit operator bool() const {
        return ptr;
    }

    R operator()(Args... args) const {
        return (*reinterpret_cast<R (*)(Args...)>(ptr))(std::forward<Args>(args)...);
    }
};

} // namespace gl
} // namespace mbgl
