#pragma once

#include <cmath>
#include <cstdint>

namespace mbgl {
namespace util {

// Computes the log2(x) rounded up to the next integer.
// (== number of bits required to store x)
uint32_t ceil_log2(uint64_t x);

} // namespace util
} // namespace mbgl
