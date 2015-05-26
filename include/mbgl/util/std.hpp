#ifndef MBGL_UTIL_STD
#define MBGL_UTIL_STD

#include <memory>
#include <type_traits>
#include <utility>

namespace mbgl {
namespace util {

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
