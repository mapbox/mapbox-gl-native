#ifndef MBGL_STYLE_VALUE
#define MBGL_STYLE_VALUE

#include <mapbox/variant.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <cstdlib>
#include <cerrno>

namespace mbgl {

typedef mapbox::util::variant<bool, int64_t, uint64_t, double, std::string> Value;

std::string toString(const Value &value);

Value parseValue(const JSValue&);

} // namespace mbgl

#endif
