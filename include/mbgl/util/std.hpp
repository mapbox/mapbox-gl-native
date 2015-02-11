#ifndef MBGL_UTIL_STD
#define MBGL_UTIL_STD

#include <memory>
#include <type_traits>
#include <utility>

namespace mbgl {
namespace util {

// C++14 backfill based on http://llvm.org/svn/llvm-project/libcxx/trunk/include/memory

namespace detail {
template<class T> struct unique_type { typedef ::std::unique_ptr<T> single; };
template<class T> struct unique_type<T[]> { typedef ::std::unique_ptr<T[]> unknown_bound; };
template<class T, size_t size> struct unique_type<T[size]> { typedef void known_bound; };
}

template<class T, class... Args>
typename detail::unique_type<T>::single make_unique(Args&&... args) {
    return ::std::unique_ptr<T>(new T(::std::forward<Args>(args)...));
}

template<class T>
typename detail::unique_type<T>::unknown_bound make_unique(size_t size) {
    return ::std::unique_ptr<T>(new typename ::std::remove_extent<T>::type[size]());
}

template<class T, class... Args>
typename detail::unique_type<T>::known_bound make_unique(Args&&...) = delete;



template <typename Container, typename ForwardIterator, typename Predicate>
void erase_if(Container &container, ForwardIterator it, const ForwardIterator end, Predicate pred) {
    while (it != end) {
        if (pred(*it)) {
            container.erase(it++);
        } else {
            ++it;
        }
    }
}

template <typename Container, typename Predicate>
void erase_if(Container &container, Predicate pred) {
    erase_if(container, container.begin(), container.end(), pred);
}

}
}

#endif
