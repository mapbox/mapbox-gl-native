#include "dtoa.hpp"

#include <rapidjson/internal/dtoa.h>

namespace mbgl {
namespace util {

std::string dtoa(double value, bool decimal) {
    std::string data;
    data.resize(25);
    auto end = rapidjson::internal::dtoa(value, const_cast<char*>(data.data()));
    auto length = end - data.data();
    if (!decimal && length >= 3 && end[-1] == '0' && end[-2] == '.') {
        // Remove trailing ".0" for integers
        length -= 2;
    }
    data.resize(length);
    return data;
}

} // namespace util
} // namespace mbgl
