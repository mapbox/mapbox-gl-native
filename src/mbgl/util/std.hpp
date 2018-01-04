#pragma once

#include <memory>
#include <type_traits>
#include <utility>

namespace mbgl {
namespace util {

template <typename Container, typename ForwardIterator, typename Predicate>
void erase_if(Container &container, ForwardIterator it, Predicate pred) {
    while (it != container.end()) {
        if (pred(*it)) {
            it = container.erase(it);
        } else {
            ++it;
        }
    }
}

template <typename Container, typename Predicate>
void erase_if(Container &container, Predicate pred) {
    erase_if(container, container.begin(), pred);
}

} // namespace util
} // namespace mbgl
