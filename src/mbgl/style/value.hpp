#ifndef MBGL_STYLE_VALUE
#define MBGL_STYLE_VALUE

#include <mbgl/util/variant.hpp>
#include <rapidjson/document.h>

#include <cstdlib>
#include <cerrno>

namespace mbgl {

typedef mapbox::util::variant<bool, int64_t, uint64_t, double, std::string> Value;

std::string toString(const Value &value);

Value parseValue(const rapidjson::Value&);

namespace util {
inline bool parseNumericString(const std::string &str, double &result) {
    char *end = nullptr;
    const char *begin = str.c_str();
    result = std::strtod(begin, &end);
    while (*end != '\0' && isspace(*end)) end++; // eat whitespace after the end
    return errno == 0 && end - begin == long(str.size());
}
}

template <typename T>
T toNumber(const Value &value) {
    if (value.is<std::string>()) {
        double val;
        return util::parseNumericString(value.get<std::string>(), val) ? val : 0;
    }
    else if (value.is<bool>()) return value.get<bool>();
    else if (value.is<int64_t>()) return value.get<int64_t>();
    else if (value.is<uint64_t>()) return value.get<uint64_t>();
    else if (value.is<double>()) return value.get<double>();
    else return 0;
}

}

#endif
