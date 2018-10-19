#pragma once

#include <mbgl/util/ignore.hpp>

#include <functional>

namespace mbgl {
namespace util {

template <class T>
void hash_combine(std::size_t& seed, const T& v) {
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <class... Args>
std::size_t hash(Args&&... args) {
    std::size_t seed = 0;
    ignore({ (hash_combine(seed, args), 0)... });
    return seed;
}

} // namespace util
} // namespace mbgl
