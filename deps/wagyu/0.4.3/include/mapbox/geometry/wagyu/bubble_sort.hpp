#pragma once

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename It, class Compare, class MethodOnSwap>
void bubble_sort(It begin, It end, Compare c, MethodOnSwap m) {
    if (begin == end) {
        return;
    }
    bool modified = false;
    auto last = end - 1;
    do {
        modified = false;
        for (auto itr = begin; itr != last; ++itr) {
            auto next = std::next(itr);
            if (!c(*itr, *next)) {
                m(*itr, *next);
                std::iter_swap(itr, next);
                modified = true;
            }
        }
    } while (modified);
}
}
}
}
