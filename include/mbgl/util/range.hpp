#include <utility>

#pragma once

namespace mbgl {

template <class T>
class Range {
public:
    constexpr Range(T min_, T max_)
        : min(std::move(min_)), max(std::move(max_)) {}

    T min;
    T max;
};

template <class T>
bool operator==(const Range<T>& a, const Range<T>& b) {
    return a.min == b.min && a.max == b.max;
}

template <class T>
bool operator!=(const Range<T>& a, const Range<T>& b) {
    return !(a == b);
}

} // namespace mbgl
