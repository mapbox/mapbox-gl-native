#pragma once

#include <mbgl/util/optional.hpp>

#include <algorithm>
#include <cassert>
#include <string>

namespace mbgl {

template <typename T>
class Enum {
public:
    using Value = std::pair<const T, const char *>;

    static const char * toString(T t) {
        auto it = std::find_if(begin, end, [&] (const auto& v) { return t == v.first; });
        assert(it != end); return it->second;
    }

    static optional<T> toEnum(const std::string& s) {
        auto it = std::find_if(begin, end, [&] (const auto& v) { return s == v.second; });
        return it == end ? optional<T>() : it->first;
    }

private:
    static const Value* begin;
    static const Value* end;
};

#define MBGL_DEFINE_ENUM(type, strings...) \
const constexpr Enum<type>::Value type##_names[] = strings; \
template <> const Enum<type>::Value* Enum<type>::begin = std::begin(type##_names); \
template <> const Enum<type>::Value* Enum<type>::end = std::end(type##_names)

} // namespace mbgl
