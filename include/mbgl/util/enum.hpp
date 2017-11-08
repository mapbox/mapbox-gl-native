#pragma once

#include <mbgl/util/optional.hpp>

#include <algorithm>
#include <cassert>
#include <string>

namespace mbgl {

template <typename T>
class Enum {
public:
    using Type = T;
    static const char * toString(T);
    static optional<T> toEnum(const std::string&);
};

#define MBGL_DEFINE_ENUM(T, values...)                                          \
                                                                                \
static const constexpr std::pair<const T, const char *> T##_names[] = values;   \
                                                                                \
template <>                                                                     \
const char * Enum<T>::toString(T t) {                                           \
    auto it = std::find_if(std::begin(T##_names), std::end(T##_names),          \
        [&] (const auto& v) { return t == v.first; });                          \
    assert(it != std::end(T##_names)); return it->second;                       \
}                                                                               \
                                                                                \
template <>                                                                     \
optional<T> Enum<T>::toEnum(const std::string& s) {                             \
    auto it = std::find_if(std::begin(T##_names), std::end(T##_names),          \
        [&] (const auto& v) { return s == v.second; });                         \
    return it == std::end(T##_names) ? optional<T>() : it->first;               \
}

} // namespace mbgl
