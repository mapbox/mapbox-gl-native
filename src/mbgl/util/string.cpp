#include <mbgl/util/string.hpp>
#include <mbgl/util/dtoa.hpp>

namespace mbgl {
namespace util {

std::string toString(float num, bool decimal) {
    return dtoa(num, decimal);
}

std::string toString(double num, bool decimal) {
    return dtoa(num, decimal);
}

std::string toString(long double num, bool decimal) {
    return dtoa(num, decimal);
}

namespace {

template <typename T>
std::string toPaddedHex(T x) {
    std::string result;
    result.resize(sizeof(T) * 2);
    for (int index = sizeof(T) * 2 - 1; index >= 0; index--) {
        const int digit = x & 0x0F;
        result[index] = '0' + digit + (digit > 9 ? 39 : 0);
        x >>= 4;
    }
    return result;
}

}

std::string toHex(size_t value) {
    return toPaddedHex(value);
}

} // namespace util
} // namespace mbgl
