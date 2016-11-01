#pragma once

namespace mbgl {

template <class T>
class Range {
public:
    constexpr Range(const T& min_, const T& max_)
        : min(min_), max(max_) {}

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
