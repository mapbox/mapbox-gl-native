#ifndef MBGL_STYLE_VALUE
#define MBGL_STYLE_VALUE

#include <mbgl/util/variant.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <cstdlib>
#include <cerrno>

namespace mbgl {

typedef variant<bool, int64_t, uint64_t, double, std::string> Value;

std::string toString(const Value &value);

Value parseValue(const JSValue&);

} // namespace mbgl

#endif
