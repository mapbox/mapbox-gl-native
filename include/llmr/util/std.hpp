#ifndef LLMR_UTIL_STD
#define LLMR_UTIL_STD

#include <memory>

namespace std {

// C++14 backfill from http://herbsutter.com/gotw/_102/
template<typename T, typename ...Args>
::std::unique_ptr<T> make_unique(Args&& ...args) {
    return ::std::unique_ptr<T>(new T(::std::forward<Args>(args)...));
}


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

#endif
