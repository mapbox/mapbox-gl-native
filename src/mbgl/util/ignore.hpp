#pragma once

#include <initializer_list>

namespace mbgl {
namespace util {

// Accept any number of parameters of any types, and do nothing with them.
// Useful for providing a context for parameter pack expansion where a legal
// expansion context is not otherwise available.
//
// See https://github.com/mapbox/cpp/blob/master/C%2B%2B%20Structural%20Metaprogramming.md
// for more details.
//
template <class... Ts> void ignore(Ts&&...) {}

// std::initializer_list overload, for situations where you need sequenced
// modifications.
//
template <class T> void ignore(const std::initializer_list<T>&) {}

// Handle the zero-argument case.
inline void ignore(const std::initializer_list<int>&) {}

} // namespace util
} // namespace mbgl
