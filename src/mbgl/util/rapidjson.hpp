#ifndef MBGL_UTIL_RAPIDJSON
#define MBGL_UTIL_RAPIDJSON

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

namespace mbgl {

using JSDocument = rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator>;
using JSValue = rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>;

} // namespace mbgl

#endif
