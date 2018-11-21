#pragma once

namespace mbgl {
namespace util {

inline constexpr uint64_t hashFNV1a(const char * str, uint64_t value = 0xcbf29ce484222325) {
    return str[0] ? hashFNV1a(str + 1, (value ^ uint64_t(str[0])) * 0x100000001b3) : value;
}

} // end namespace util
} // end namespace mbgl
