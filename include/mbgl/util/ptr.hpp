#ifndef MBGL_UTIL_PTR
#define MBGL_UTIL_PTR

#include <memory>
#include <cassert>

namespace mbgl {
namespace util {

template <typename T>
class ptr : public ::std::shared_ptr<T> {
public:
    template <typename... Args>
    inline ptr(Args &&... args)
        : ::std::shared_ptr<T>(::std::forward<Args>(args)...) {}

    inline auto operator->() const -> decltype(this->::std::shared_ptr<T>::operator->()) {
        assert(*this);
        return ::std::shared_ptr<T>::operator->();
    }
    inline auto operator*() const -> decltype(this->::std::shared_ptr<T>::operator*()) {
        assert(*this);
        return ::std::shared_ptr<T>::operator*();
    }
};
}
}

#endif