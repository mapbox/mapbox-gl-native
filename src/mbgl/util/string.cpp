#include <mbgl/util/string.hpp>
#include <mbgl/util/dtoa.hpp>

namespace mbgl {
namespace util {

std::string toString(float num) {
    return dtoa(num);
}

std::string toString(double num) {
    return dtoa(num);
}

std::string toString(long double num) {
    return dtoa(num);
}

} // namespace util
} // namespace mbgl
