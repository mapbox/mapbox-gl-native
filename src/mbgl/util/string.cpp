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
}

} // namespace util
} // namespace mbgl
