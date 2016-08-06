#pragma once

#include <memory>
#include <cassert>

namespace mbgl {
namespace util {

template <typename T>
class ptr : public ::std::shared_ptr<T> {
public:
    template <typename... Args>
    ptr(Args &&... args)
        : ::std::shared_ptr<T>(::std::forward<Args>(args)...) {}

    auto operator->() const -> decltype(this->::std::shared_ptr<T>::operator->()) {
        assert(*this);
        return ::std::shared_ptr<T>::operator->();
    }
    auto operator*() const -> decltype(this->::std::shared_ptr<T>::operator*()) {
        assert(*this);
        return ::std::shared_ptr<T>::operator*();
    }
};
} // namespace util
} // namespace mbgl
