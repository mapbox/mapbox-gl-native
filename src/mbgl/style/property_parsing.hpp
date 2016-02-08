#ifndef MBGL_PROPERTY_PARSING
#define MBGL_PROPERTY_PARSING

#include <mbgl/style/types.hpp>

#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/optional.hpp>

#include <functional>

namespace mbgl {

template <typename T>
using optional = optional<T>;

template <typename T>
optional<T> parseProperty(const char* name, const JSValue&);

} // namespace mbgl

#endif
