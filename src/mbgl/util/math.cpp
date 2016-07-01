#include <mbgl/util/math.hpp>

namespace mbgl {
namespace util {

// From http://stackoverflow.com/questions/3272424/compute-fast-log-base-2-ceiling
uint32_t ceil_log2(uint64_t x) {
    static const uint64_t t[6] = {0xFFFFFFFF00000000, 0x00000000FFFF0000,
                                  0x000000000000FF00, 0x00000000000000F0,
                                  0x000000000000000C, 0x0000000000000002};
    uint32_t y = (((x & (x - 1)) == 0) ? 0 : 1);
    uint32_t j = 32;

    for (const auto& i : t) {
        const uint32_t k = (((x & i) == 0) ? 0 : j);
        y += k;
        x >>= k;
        j >>= 1;
    }

    return y;
}

double log2(double x) {
// log2() is producing wrong results on ARMv5 binaries
// running on ARMv7+ CPUs.
#if defined(__ANDROID__)
    return std::log(x) / 0.6931471805599453; // log(x) / log(2)
#else
    return ::log2(x);
#endif
}

} // namespace util
} // namespace mbgl
